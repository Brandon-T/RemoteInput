//
// Created by Brandon on 2020-05-02.
//

#include "Injector.hxx"

#if defined(__APPLE__)
#include <dlfcn.h>
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <mach-o/loader.h>
#include <mach-o/dyld_images.h>
#import <mach-o/nlist.h>

#include <cstdint>
#include <string>
#endif

#if defined(__APPLE__)
//Strip the module name from its path
auto strip_path = [](const std::string &path_to_strip) -> std::string {
    std::string::size_type pos = path_to_strip.find_last_of("\\/");
    if (pos != std::string::npos)
    {
        //Strip path components
        std::string path = path_to_strip.substr(pos + 1);
        pos = path.find_last_of('.');

        //Strip extension
        return pos > 0 && pos != std::string::npos ? path.substr(0, pos) : path;
    }
    return path_to_strip;
};

//Calls dlopen inside of a pthread in the remote target
auto remote_load_library = [](std::size_t* instructions_size) -> std::uint8_t* {
    static std::uint8_t assembly[] = {
            0x50,                                              //push rax
            0x48, 0x89, 0xF1,                                  //mov rcx, rsi
            0x48, 0x8B, 0x17,                                  //mov rdx, qword ptr[rdi]        //_pthread_callback = data[0]
            0x48, 0x8B, 0x47, 0x08,                            //mov rax, qword ptr[rdi + 8]    //_pthread_create_from_mach_thread = data[1]
            0x48, 0xC7, 0x04, 0x24, 0x00, 0x00, 0x00, 0x00,    //mov qword ptr[rsp], 0          //pthread thread_id = 0
            0x48, 0x89, 0xE7,                                  //mov rdi, rsp
            0x31, 0xF6,                                        //xor esi, esi
            0xFF, 0xD0,                                        //call rax                       //call _pthread_create_from_mach_thread(&thread_id, nullptr, _pthread_callback, param)
            0x48, 0x8B, 0x04, 0x24,                            //mov rax, qword ptr[rsp]        //return pthread_id
            0x59,                                              //pop rcx
            0xEB, 0xFE,                                        //jmp 0x00                       //Do not return to callee
            0xC3,                                              //ret
            0x90,
            0x90,
            0x90,                                              //nop
            0x90,
            0x90,
            0x48, 0xB8, 0xEF, 0xBE, 0xAD, 0xDE, 0x00, 0x00, 0x00, 0x00,   //movabs rax, 0x0000DEADBEEF     //address of dlopen
            0xBE, 0x01, 0x00, 0x00, 0x00,                                 //mov esi, RTLD_LAZY             //RTLD_LAZY = 0x1 -- RTLD_NOW = 0x02
            0xFF, 0xE0                                                    //jmp rax                        //call dlopen(pthread_parameters, RTLD_LAZY)

            /*pthread_t LoadLibrary(void** data, void* param)
            {
                void* (*_dlopen_pointer)(void* param) = (decltype(_dlopen_pointer))(data[0]);
                int (*_pthread_create_from_mach_thread)(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *) = (decltype(_pthread_create_from_mach_thread))(data[1]);

                pthread_t thread_id = 0;
                _pthread_create_from_mach_thread(&thread_id, nullptr, _dlopen_pointer, param);
                return thread_id;
            }

            void* _dlopen_pointer(void* param)
            {
                decltype(dlopen)* _dlopen = (decltype(dlopen)*)0xa000deadbeef;
                return _dlopen((const char*)param, RTLD_NOW);
            }*/
    };
    *instructions_size = sizeof(assembly);
    return &assembly[0];
};

//Find address of a dynamic library
auto find_library = [](mach_port_t task, const char* library) -> std::intptr_t {
    task_dyld_info_data_t info;
    mach_msg_type_number_t count = TASK_DYLD_INFO_COUNT;
    task_info(task, TASK_DYLD_INFO, reinterpret_cast<task_info_t>(&info), &count);

    //Get the loaded dylibs/images
    dyld_all_image_infos infos = {0};
    mach_vm_size_t size = info.all_image_info_size;
    int err = mach_vm_read_overwrite(task, info.all_image_info_addr, info.all_image_info_size, reinterpret_cast<mach_vm_address_t>(&infos), &size);

    if (size <= 0 || err != KERN_SUCCESS)
    {
        return NULL;
    }

    //Get the info for each dylib/image
    size = sizeof(dyld_all_image_infos) * infos.infoArrayCount;
    std::unique_ptr<dyld_image_info[]> image_infos = std::make_unique<dyld_image_info[]>(size);
    err = mach_vm_read_overwrite(task, reinterpret_cast<mach_vm_address_t>(infos.infoArray), size, reinterpret_cast<mach_vm_address_t>(image_infos.get()), &size);

    if (size <= 0 || err != KERN_SUCCESS)
    {
        return NULL;
    }

    //Get each image's file paths
    for (std::uint32_t i = 0; i < infos.infoArrayCount; ++i)
    {
        char buffer[512] = {0};
        mach_vm_size_t size = sizeof(buffer);

        int err = mach_vm_read_overwrite(task, reinterpret_cast<mach_vm_address_t>(image_infos[i].imageFilePath), size, reinterpret_cast<mach_vm_address_t>(&buffer[0]), &size);
        if (err == KERN_SUCCESS && size > 0)
        {
            std::string path = strip_path(buffer);
            if (!strcasecmp(path.c_str(), library))
            {
                return reinterpret_cast<std::uintptr_t>(image_infos[i].imageLoadAddress);
            }
        }
    }

    return NULL;
};

//Find address of a symbol within a dynamic library
auto find_symbol = [](mach_port_t task, mach_vm_address_t library_header_address, const char* symbol) -> std::intptr_t {
    mach_vm_size_t size = sizeof(mach_header);
    struct mach_header header = {0};
    mach_vm_read_overwrite(task, library_header_address, size, reinterpret_cast<mach_vm_address_t>(&header), &size);

    //64-bit mach-o module..
    if (header.magic == MH_MAGIC_64)
    {
        load_command command = {0};
        mach_vm_size_t size = sizeof(command);

        //stackoverflow.com/a/40793165/1462718
        //stackoverflow.com/questions/20481058/find-pathname-from-dlopen-handle-on-osx
        mach_vm_address_t seg_linkedit_addr = reinterpret_cast<mach_vm_address_t>(nullptr);
        mach_vm_address_t seg_text_addr = reinterpret_cast<mach_vm_address_t>(nullptr);
        mach_vm_address_t symtab_addr = reinterpret_cast<mach_vm_address_t>(nullptr);
        mach_vm_address_t load_command_address = library_header_address + sizeof(struct mach_header_64);

        //Iterate through all the load commands in the header..
        for (uint32_t i = 0; i < header.ncmds; ++i)
        {
            mach_vm_read_overwrite(task, load_command_address, size, reinterpret_cast<mach_vm_address_t>(&command), &size);
            switch(command.cmd)
            {
                case LC_SEGMENT:
                case LC_SEGMENT_64:
                {
                    char name[512] = {0};
                    mach_vm_size_t size = sizeof(name);

                    //Read the name of each segment in the header
                    std::intptr_t segname_offset = offsetof(segment_command_64, segname);
                    mach_vm_read_overwrite(task, load_command_address + segname_offset, size, reinterpret_cast<mach_vm_address_t>(&name[0]), &size);

                    if (std::string(name) == SEG_TEXT)
                    {
                        seg_text_addr = load_command_address;
                    }
                    else if (std::string(name) == SEG_LINKEDIT)
                    {
                        seg_linkedit_addr = load_command_address;
                    }
                }
                    break;

                case LC_SYMTAB:
                {
                    symtab_addr = load_command_address;
                }
                    break;
            }

            load_command_address += command.cmdsize;
        }

        if (!seg_text_addr || !seg_linkedit_addr || !symtab_addr)
        {
            return NULL;
        }

        //Read each segment
        segment_command_64 seg_linkedit = {0};
        segment_command_64 seg_text = {0};
        symtab_command symtab = {0};

        size = sizeof(seg_linkedit);
        mach_vm_read_overwrite(task, seg_linkedit_addr, size, reinterpret_cast<mach_vm_address_t>(&seg_linkedit), &size);

        size = sizeof(seg_text);
        mach_vm_read_overwrite(task, seg_text_addr, size, reinterpret_cast<mach_vm_address_t>(&seg_text), &size);

        size = sizeof(symtab);
        mach_vm_read_overwrite(task, symtab_addr, size, reinterpret_cast<mach_vm_address_t>(&symtab), &size);

        //ASLR slide calculations
        std::intptr_t file_slide = seg_linkedit.vmaddr - seg_text.vmaddr - seg_linkedit.fileoff;
        std::intptr_t strings = library_header_address + symtab.stroff + file_slide;
        std::intptr_t sym_addr = library_header_address + symtab.symoff + file_slide;

        //Iterate through all the symbols in that segment
        for (uint32_t i = 0; i < symtab.nsyms; ++i)
        {
            nlist_64 sym = {0};
            size = sizeof(sym);
            mach_vm_read_overwrite(task, sym_addr, size, reinterpret_cast<mach_vm_address_t>(&sym), &size);

            if (/*(sym.n_type & N_EXT) == N_EXT &&*/ sym.n_value)
            {
                char name[512] = {0};
                mach_vm_size_t size = sizeof(name);

                std::intptr_t symname_offset = strings + sym.n_un.n_strx;
                mach_vm_read_overwrite(task, symname_offset, size, reinterpret_cast<mach_vm_address_t>(&name[0]), &size);

                if (std::string(name).substr(1) == symbol)
                {
                    if (sym.n_value < 0x1000)
                    {
                        return sym.n_value + library_header_address;
                    }

                    return sym.n_value + library_header_address; //kx86_64DefaultBaseAddress
                }
            }

            sym_addr += size;
        }
    }

    return NULL;
};

bool Injector::Inject(std::string module_path, pid_t pid, void* bootstrap) noexcept
{
    std::size_t assembly_size = 0;
    std::uint8_t* assembly = remote_load_library(&assembly_size);

    //Retrieve a task port for the remote process..
    mach_port_t	remote_task = 0;
    mach_error_t err = task_for_pid(mach_task_self(), pid, &remote_task);
    if (err == 5)
    {
        fprintf(stderr, "Could not access task for pid %d. You probably need to add user to procmod group\n", pid);
        return false;
    }

    std::uint64_t stack_size = 16 * 1024;

    //Not needed right now since we use the same offsets from our own process for the target process..
    //std::intptr_t dlopen_address = find_symbol(remote_task, find_library(remote_task, "libdyld"), "dlopen");
    //std::intptr_t pthread_create_from_mach_thread = find_symbol(remote_task, find_library(remote_task, "libsystem_pthread"), "pthread_create_from_mach_thread");

    //If the library is already loaded
    if (find_library(remote_task, strip_path(module_path).c_str()))
    {
        return true;
    }

    // Allocate and write the path size..
    mach_vm_address_t remote_path = reinterpret_cast<mach_vm_address_t>(nullptr);
    mach_vm_allocate(remote_task, &remote_path, module_path.size() + 1, VM_FLAGS_ANYWHERE);
    mach_vm_protect(remote_task, remote_path, module_path.size() + 1, 0, VM_PROT_EXECUTE | VM_PROT_WRITE | VM_PROT_READ);
    mach_vm_write(remote_task, remote_path, reinterpret_cast<mach_vm_offset_t>(module_path.c_str()), static_cast<mach_msg_type_number_t>(module_path.size()));

    //Update our dlopen address..
    void* ptr = (void*)dlopen;
    memcpy(&assembly[assembly_size - 15], (void*)&ptr, sizeof(ptr));

    //Allocate and write our remote code
    mach_vm_address_t remote_code = reinterpret_cast<mach_vm_address_t>(nullptr);
    mach_vm_allocate(remote_task, &remote_code, assembly_size, VM_FLAGS_ANYWHERE);
    mach_vm_protect(remote_task, remote_code, assembly_size, 0, VM_PROT_EXECUTE | VM_PROT_WRITE | VM_PROT_READ);
    mach_vm_write(remote_task, remote_code, reinterpret_cast<mach_vm_offset_t>(&assembly[0]), static_cast<mach_msg_type_number_t>(assembly_size));

    //Allocate remote stack..
    mach_vm_address_t remote_stack = reinterpret_cast<mach_vm_address_t>(nullptr);
    mach_vm_allocate(remote_task, &remote_stack, stack_size, VM_FLAGS_ANYWHERE);
    mach_vm_protect(remote_task, remote_stack, stack_size, true, VM_PROT_READ | VM_PROT_WRITE);

    //Allocate & write parameters..
    void* parameters[] = {
            (void*)(remote_code + (assembly_size - 18)),
            (void*)dlsym(RTLD_DEFAULT, "pthread_create_from_mach_thread")
    };

    mach_vm_address_t remote_parameters = reinterpret_cast<mach_vm_address_t>(nullptr);
    mach_vm_allocate(remote_task, &remote_parameters, sizeof(parameters), VM_FLAGS_ANYWHERE);
    mach_vm_write(remote_task, remote_parameters, reinterpret_cast<mach_vm_offset_t>(&parameters[0]), static_cast<mach_msg_type_number_t>(sizeof(parameters)));

    //Offset stack pointer..
    mach_vm_address_t local_stack = remote_stack;
    remote_stack += (stack_size / 2);
    remote_stack -= 8;

    x86_thread_state64_t state = {0};
    memset(&state, 0, sizeof(state));

    //Fake return address on the stack
    std::intptr_t return_address_stack[] = { 0x00000DEADBEA7DAD };
    mach_vm_write(remote_task, remote_stack, reinterpret_cast<mach_vm_offset_t>(&return_address_stack[0]), sizeof(return_address_stack));

    //Parameter order for x86_64: RDI, RSI, RDX, RCX, R8, R9
    state.__rdi = remote_parameters; //pointers to functions
    state.__rsi = remote_path;       //path of module
    state.__rdx = local_stack;       //userp
    state.__rip = remote_code;       //code to execute
    state.__rax = local_stack;       //return address is the top of our stack

    //Set remote Stack Pointer
    state.__rsp = remote_stack;
    state.__rbp = remote_stack;

    //Create our remote thread
    thread_act_t thread;
    thread_create_running(remote_task, x86_THREAD_STATE64, (thread_state_t) &state, x86_THREAD_STATE64_COUNT, &thread);

    //Check to make sure our remote thread was not suspended
    thread_basic_info_data_t basic_info;
    mach_msg_type_number_t info_count = THREAD_BASIC_INFO_COUNT;
    err = thread_info(thread, THREAD_BASIC_INFO, (thread_info_t)&basic_info, &info_count);
    if (err == KERN_SUCCESS)
    {
        bool is_suspended = (basic_info.suspend_count > 0);
        if (!is_suspended)
        {
            //Retrieve our remote thread state.
            x86_thread_state64_t state;
            mach_msg_type_number_t count = x86_THREAD_STATE64_COUNT;
            thread_get_state(thread, x86_THREAD_STATE64, reinterpret_cast<thread_state_t>(&state), &count);

            //Get the return value of our function call in rax.. aka pthread_t thread_id;
            pthread_t rax = (pthread_t)state.__rax;

            if (rax != 0)
            {
                //We should cleanup.. but the remote process will deallocate everything once it exists anyway..
                //And cleaning up before the thread is complete causes crashes.. Not sure why yet..

                //thread_suspend(thread);
                //thread_terminate(thread);

                /*sleep(1);
                mach_vm_deallocate(remote_task, remote_parameters, sizeof(parameters));
                mach_vm_deallocate(remote_task, remote_stack, stack_size);
                mach_vm_deallocate(remote_task, remote_code, sizeof(assembly));
                mach_vm_deallocate(remote_task, remote_path, module_path.size() + 1);*/
                return true;
            }
        }
    }

    return false;
}
#endif
cmake_minimum_required(VERSION 3.15)
project(THIRD_PARTY_LIBRARIES VERSION 1.0.0 DESCRIPTION "Third Party")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)



# ----------------------- INCLUDE_DIRECTORIES -----------------------
IF(WIN32)
    set(INCLUDE_DIRECTORIES
            min_hook/include
            min_hook/src
            min_hook/src/hde)
ELSEIF(APPLE)
    set(INCLUDE_DIRECTORIES
            rd_route/include
            mach_inject/include)
ELSE()
    set(INCLUDE_DIRECTORIES
            linux_detours/include)
ENDIF()


# ----------------------------- LINKER -----------------------------
IF(WIN32)
    set(LIBRARIES_LIST
            kernel32)
ELSEIF(APPLE)
    set(LIBRARIES_LIST
            dl
            pthread)
ELSE()
    set(LIBRARIES_LIST
            dl
            pthread)
ENDIF()


# ---------------------------- SOURCES ----------------------------
IF(WIN32)
    set(LIB_SRC_LIST
            min_hook/include/MinHook.h
            min_hook/src/hde/hde32.c
            min_hook/src/hde/hde32.h
            min_hook/src/hde/hde64.c
            min_hook/src/hde/hde64.h
            min_hook/src/hde/pstdint.h
            min_hook/src/hde/table32.h
            min_hook/src/hde/table64.h
            min_hook/src/buffer.c
            min_hook/src/buffer.h
            min_hook/src/hook.c
            min_hook/src/trampoline.c
            min_hook/src/trampoline.h)
ELSEIF(APPLE)
    set(LIB_SRC_LIST
            rd_route/include/rd_route.h
            rd_route/src/rd_route.c
            mach_inject/include/mach_inject.h
            mach_inject/src/mach_inject.c)
ELSE()
    set(LIB_SRC_LIST
            linux_detours/include/detours.h
            #linux_detours/include/limits.h
            linux_detours/include/plthook.h
            linux_detours/include/types.h
            linux_detours/src/barrier.cpp
            linux_detours/src/detours.cpp
            linux_detours/src/disasm.cpp
            linux_detours/src/plthook_elf.cpp
            linux_detours/src/trampoline_x86.cpp
            linux_detours/src/trampoline_arm.cpp)
ENDIF()

set(SRC_LIST
        ${LIB_SRC_LIST}
        main.hxx
        main.cxx)


# ---------------------------- COMPILE ----------------------------
add_library(${PROJECT_NAME} OBJECT ${SRC_LIST})
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRECTORIES})

IF(WIN32)
    #target_compile_definitions(${PROJECT_NAME} PRIVATE _AMD64_=1 _UNIX64=1)

    target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:DEBUG>:-Wno-multichar -Wno-null-conversion -g -O0 -fno-omit-frame-pointer>
            $<$<CONFIG:RELEASE>:-Wno-multichar -Wno-null-conversion -s -O0 -fno-omit-frame-pointer>)
    target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:DEBUG>:-static -static-libgcc -static-libstdc++>
            $<$<CONFIG:RELEASE>:-s -static -static-libgcc -static-libstdc++>)
ELSEIF(APPLE)
    #target_compile_definitions(${PROJECT_NAME} PRIVATE _AMD64_=1 _UNIX64=1)

    target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:DEBUG>:-Wno-multichar -Wno-null-conversion -g -O0 -fno-omit-frame-pointer>
            $<$<CONFIG:RELEASE>:-Wno-multichar -Wno-null-conversion -O0 -fno-omit-frame-pointer>)
    target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:DEBUG>:-static>
            $<$<CONFIG:RELEASE>:-s -static>)
ELSE()
    target_compile_definitions(${PROJECT_NAME} PRIVATE _AMD64_=1 _UNIX64=1)

    target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:DEBUG>:-Wno-multichar -Wno-null-conversion -g -O0 -fno-omit-frame-pointer>
            $<$<CONFIG:RELEASE>:-Wno-multichar -Wno-null-conversion -s -O0 -fno-omit-frame-pointer>)
    target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:DEBUG>:-static-libgcc -static-libstdc++>
            $<$<CONFIG:RELEASE>:-s -static-libgcc -static-libstdc++>)
ENDIF()

target_link_libraries(${PROJECT_NAME}  ${LIBRARIES_LIST})

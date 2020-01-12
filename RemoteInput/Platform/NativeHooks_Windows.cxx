#include "NativeHooks.hxx"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <gl/gl.h>
#include <gl/glext.h>
#include <GL/wglext.h>

#include <memory>
#include <thread>
#include <mutex>
#include "JavaInternal.hxx"
#include "EIOS.hxx"
#include "Graphics.hxx"
#endif

#if __has_include("MinHook.h")
#include "MinHook.h"
#endif


#if defined(_WIN32) || defined(_WIN64)
typedef void (__stdcall *JavaNativeBlit_t)(JNIEnv *env, jobject joSelf, jobject srcData, jobject dstData, jobject clip, jint srcx, jint srcy, jint dstx, jint dsty, jint width, jint height, jint rmask, jint gmask, jint bmask, jboolean needLut);
JavaNativeBlit_t o_JavaNativeBlit;
#endif


#if defined(_WIN32) || defined(_WIN64)
void __stdcall JavaNativeBlit(JNIEnv *env, jobject joSelf, jobject srcData, jobject dstData, jobject clip, jint srcx, jint srcy, jint dstx, jint dsty, jint width, jint height, jint rmask, jint gmask, jint bmask, jboolean needLut)
{
    extern std::unique_ptr<ControlCenter> control_center;
	if (control_center)
	{
		typedef SurfaceDataOps* (__stdcall *SurfaceData_GetOps)(JNIEnv *env, jobject sData);
		static SurfaceData_GetOps GetOps = (SurfaceData_GetOps)GetProcAddress(GetModuleHandle("awt.dll"), "_SurfaceData_GetOps@8");

		if (!GetOps)
		{
			return;
		}

		SurfaceDataOps* srcOps = GetOps(env, srcData);
		SurfaceDataRasInfo srcInfo = {0};
		srcInfo.bounds.x1 = srcx;
		srcInfo.bounds.y1 = srcy;
		srcInfo.bounds.x2 = srcx + width;
		srcInfo.bounds.y2 = srcy + height;

		if (srcOps->Lock(env, srcOps, &srcInfo, needLut ? (SD_LOCK_READ | SD_LOCK_LUT) : SD_LOCK_READ) == SD_SUCCESS)
		{
			srcOps->GetRasInfo(env, srcOps, &srcInfo);
			if (srcInfo.rasBase)
			{
				void* rasBase = reinterpret_cast<std::uint8_t*>(srcInfo.rasBase) + (srcInfo.scanStride * srcy) + (srcInfo.pixelStride * srcx);
				bool isRasterAligned = !(srcInfo.scanStride & 0x03);

				control_center->update_dimensions(width, height);
				std::uint8_t* dest = control_center->get_image();

				//Render to Shared Memory
				if (dest)
				{
					if (isRasterAligned)
					{
						memcpy(dest, rasBase, (srcInfo.scanStride / srcInfo.pixelStride) * height * srcInfo.pixelStride);
					}
					else
					{
						for (int i = 0; i < height; ++i)
						{
							int offset = (srcInfo.scanStride / srcInfo.pixelStride) * i;
							memcpy(dest + offset, rasBase, (srcInfo.scanStride / srcInfo.pixelStride));
							rasBase = static_cast<void*>(reinterpret_cast<std::uint8_t*>(rasBase) + srcInfo.scanStride);
						}
					}
				}

				//Render Debug Graphics
				if (control_center->get_debug_graphics())
				{
					std::uint8_t* src = control_center->get_debug_image();
					if (src)
					{
						rasBase = reinterpret_cast<std::uint8_t*>(srcInfo.rasBase) + (srcInfo.scanStride * srcy) + (srcInfo.pixelStride * srcx);
						draw_image(rasBase, src, width, height, srcInfo.pixelStride);
					}
				}
				
				//Render Cursor
				std::int32_t x = -1;
				std::int32_t y = -1;
				control_center->get_applet_mouse_position(&x, &y);

				if (x > -1 && y > -1)
				{
					rasBase = reinterpret_cast<std::uint8_t*>(srcInfo.rasBase) + (srcInfo.scanStride * srcy) + (srcInfo.pixelStride * srcx);
					draw_circle(x, y, 4, rasBase, width, height, srcInfo.pixelStride, true);
				}

				if (srcOps->Release)
				{
					srcOps->Release(env, srcOps, &srcInfo);
				}
			}

			if (srcOps->Unlock)
			{
				srcOps->Unlock(env, srcOps, &srcInfo);
			}
		}
	}

    //Original
    o_JavaNativeBlit(env, joSelf, srcData, dstData, clip, srcx, srcy, dstx, dsty, width, height, rmask, gmask, bmask, needLut);
}
#endif

#if defined(_WIN32) || defined(_WIN64)
BOOL (__stdcall *o_SwapBuffers)(HDC ctx);
void (__stdcall *o_glDrawPixels)(GLsizei width, GLsizei height, GLenum format, GLenum type, const void* data);
void (__stdcall *glGenBuffers) (GLsizei n, GLuint *buffers);
void (__stdcall *glDeleteBuffers) (GLsizei n,  const GLuint* buffers);
void (__stdcall *glBindBuffer) (GLenum target, GLuint buffer);
void (__stdcall *glBufferData) (GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
void* (__stdcall *glMapBuffer)(GLenum target, GLenum access);
GLboolean (__stdcall *glUnmapBuffer)(GLenum target);

void LoadOpenGLExtensions()
{
    static std::once_flag token = {};
    std::call_once(token, [&]{
        glGenBuffers = reinterpret_cast<decltype(glGenBuffers)>(wglGetProcAddress("glGenBuffers"));
        glDeleteBuffers = reinterpret_cast<decltype(glDeleteBuffers)>(wglGetProcAddress("glDeleteBuffers"));
        glBindBuffer = reinterpret_cast<decltype(glBindBuffer)>(wglGetProcAddress("glBindBuffer"));
        glBufferData = reinterpret_cast<decltype(glBufferData)>(wglGetProcAddress("glBufferData"));
        glMapBuffer = reinterpret_cast<decltype(glMapBuffer)>(wglGetProcAddress("glMapBuffer"));
        glUnmapBuffer = reinterpret_cast<decltype(glUnmapBuffer)>(wglGetProcAddress("glUnmapBuffer"));
    });
}

void GeneratePixelBuffers(void* ctx, GLuint (&pbo)[2], GLint width, GLint height, GLint stride)
{
	static int w = 0;
	static int h = 0;

	#if defined(__APPLE__)
	CGLContextObj CGL_MACRO_CONTEXT = static_cast<CGLContextObj>(ctx);
	#endif

	//Buffer size changed
	if (w != width && h != height)
	{
		//If buffers already exist, clean them up
		if (pbo[1] != 0)
		{
			glDeleteBuffers(2, pbo);
		}

		//Generate buffers
		glGenBuffers(2, pbo);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo[0]);
		glBufferData(GL_PIXEL_PACK_BUFFER, width * height * stride, 0, GL_STREAM_READ);

		glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo[1]);
		glBufferData(GL_PIXEL_PACK_BUFFER, width * height * stride, 0, GL_STREAM_READ);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	}
}

void ReadPixelBuffers(void* ctx, GLubyte* dest, GLuint (&pbo)[2], GLint width, GLint height, GLint stride)
{
	static int index = 0;
	static int nextIndex = 0;

	#if defined(__APPLE__)
	CGLContextObj CGL_MACRO_CONTEXT = static_cast<CGLContextObj>(ctx);
	#endif

	//Swap indices
	index = (index + 1) % 2;
	nextIndex = (index + 1) % 2;

	//read back-buffer.
	glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo[index]);
	glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo[nextIndex]);

	void* data = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

	if (data)
	{
		memcpy(dest, data, width * height * 4);
		data = nullptr;
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
	}
	else
	{
		glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, dest);
	}

	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void __stdcall mglDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, const void* data)
{
    extern std::unique_ptr<ControlCenter> control_center;

	if (control_center)
	{
	    int src_x = 0;
	    int src_y = 0;
	    int bytes_per_pixel = 4; //TODO: Parse format
		int stride = width * bytes_per_pixel;
        void *rasBase = static_cast<std::uint8_t*>(const_cast<void*>(data)) + (stride * src_y) + (bytes_per_pixel * src_x);

		control_center->update_dimensions(width, height);
		
		//Render to Shared Memory
		std::uint8_t* dest = control_center->get_image();
		if (dest)
		{
			memcpy(dest, rasBase, (stride / bytes_per_pixel) * height * bytes_per_pixel);
		}

		//Render Debug Graphics
		if (control_center->get_debug_graphics())
		{
			std::uint8_t* src = control_center->get_debug_image();
			if (src)
			{
				draw_image(rasBase, src, width, height, bytes_per_pixel);
			}
		}
		
		//Render Cursor
		std::int32_t x = -1;
		std::int32_t y = -1;
		control_center->get_applet_mouse_position(&x, &y);

		if (x > -1 && y > -1)
		{
			draw_circle(x, y, 3, rasBase, width, height, bytes_per_pixel, true);
		}
	}

    o_glDrawPixels(width, height, format, type, data);
}

BOOL __stdcall mSwapBuffers(HDC hdc)
{
	extern std::unique_ptr<ControlCenter> control_center;

	if (control_center)
	{
		static GLint ViewPort[4] = {0};
		static GLuint pbo[2] = {0};

		glGetIntegerv(GL_VIEWPORT, ViewPort);
		GLint width = ViewPort[2] - ViewPort[0];
		GLint height = ViewPort[3] - ViewPort[1];

		if (width >= 765 && height >= 553)
		{
			control_center->update_dimensions(width, height);
			
			//Render to Shared Memory
			std::uint8_t* dest = control_center->get_image();
			if (dest)
			{
				LoadOpenGLExtensions();
				GeneratePixelBuffers(hdc, pbo, width, height, 4);
				ReadPixelBuffers(hdc, dest, pbo, width, height, 4);
				FlipImageVertically(width, height, dest);
			}
			
			//Render Debug Graphics
			if (control_center->get_debug_graphics())
			{
				std::uint8_t* src = control_center->get_debug_image();
				if (src)
				{
					gl_draw_image(hdc, src, 0, 0, width, height, 4);
				}
			}
			
			//Render Cursor
			std::int32_t x = -1;
			std::int32_t y = -1;
			control_center->get_applet_mouse_position(&x, &y);

			if (x > -1 && y > -1)
			{
				glColor4ub(0xFF, 0x00, 0x00, 0xFF);
				gl_draw_point(hdc, x, y, 0, 4);
			}
		}
	}

	//Original
	return o_SwapBuffers(hdc);
}
#endif // defined

#if defined(_WIN32) || defined(_WIN64)
#include <d3d9.h>

typedef HRESULT (__stdcall *Device_Present_t)(IDirect3DDevice9* device, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
Device_Present_t o_DevicePresent;

HRESULT __stdcall Device_Present(IDirect3DDevice9* device, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)
{
    return o_DevicePresent(device, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

/*HRESULT __stdcall SwapChain_Present(IDirect3DSwapChain9* swapChain, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion, DWORD dwFlags)
{

}*/
#endif // defined


#if defined(_WIN32) || defined(_WIN64)
DWORD FindPattern(DWORD dwAddress, DWORD dwLen, BYTE *bMask, const char* szMask)
{
    auto bCompare = [](const BYTE* pData, const BYTE* bMask, const char* szMask) -> bool {
        for(; *szMask; ++szMask, ++pData, ++bMask)
        {
            if(*szMask == 'x' && *pData != *bMask)
            {
                return 0;
            }
        }
        return (*szMask) == 0;
    };

    for(DWORD i = 0; i < dwLen; ++i)
    {
        if (bCompare(reinterpret_cast<std::uint8_t*>(dwAddress + i), bMask, szMask))
        {
            return static_cast<DWORD>(dwAddress + i);
        }
    }
    return 0;
}

void* DetourFunction(std::uint8_t* OrigFunc, std::uint8_t* HookFunc, int JumpLength)
{
    DWORD dwProt = 0;
    std::uint8_t* jmp = new std::uint8_t[JumpLength + 5];
    VirtualProtect(OrigFunc, JumpLength, PAGE_READWRITE, &dwProt);
    memcpy(jmp, OrigFunc, JumpLength);

    jmp += JumpLength;
    jmp[0] = 0xE9;
    *reinterpret_cast<DWORD*>(jmp + 1) = static_cast<DWORD>(OrigFunc + JumpLength - jmp) - 5;
    memset(OrigFunc, 0x90, JumpLength);

    OrigFunc[0] = 0xE9;
    *reinterpret_cast<DWORD*>(OrigFunc + 1) = static_cast<DWORD>(HookFunc - OrigFunc) - 5;
    VirtualProtect(OrigFunc, JumpLength, dwProt, &dwProt);
    return (jmp - JumpLength);
}
#endif // defined

#if defined(_WIN32) || defined(_WIN64)
void InitialiseHooks()
{
    std::thread([]{
		if (!GetModuleHandle("awt.dll"))
		{
			return;
		}

		JavaNativeBlit_t original = (JavaNativeBlit_t)GetProcAddress(GetModuleHandle("awt.dll"), "_Java_sun_java2d_windows_GDIBlitLoops_nativeBlit@60");

		MH_Initialize();
		MH_CreateHook(reinterpret_cast<void*>(original), reinterpret_cast<void*>(&JavaNativeBlit), reinterpret_cast<void**>(&o_JavaNativeBlit));
		MH_EnableHook(reinterpret_cast<void*>(original));
    }).detach();

    std::thread([]{
		if (!GetModuleHandle("opengl32.dll"))
		{
			return;
		}

		decltype(o_glDrawPixels) original = (decltype(o_glDrawPixels))GetProcAddress(GetModuleHandle("opengl32.dll"), "glDrawPixels");

		MH_Initialize();
		MH_CreateHook(reinterpret_cast<void*>(original), reinterpret_cast<void*>(&mglDrawPixels), reinterpret_cast<void**>(&o_glDrawPixels));
		MH_EnableHook(reinterpret_cast<void*>(original));
    }).detach();

    /*std::thread([]{
        auto start = std::chrono::high_resolution_clock::now();
        while(!GetModuleHandle("d3d9.dll"))
        {
            if (elapsed_time<std::chrono::milliseconds>(start) >= 10)
            {
                return;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        DWORD* VTable = nullptr;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        static std::uint8_t bMask[] = {0xC7, 0x06, 0x00, 0x00, 0x00, 0x00, 0x89, 0x86, 0x00, 0x00, 0x00, 0x00, 0x89, 0x86};
        DWORD Address = FindPattern(reinterpret_cast<DWORD>(GetModuleHandle("d3d9.dll")), 0x128000, bMask, "xx????xx????xx");
        memcpy(&VTable, reinterpret_cast<void*>(Address + 2), 4);

        o_DevicePresent = reinterpret_cast<Device_Present_t>(VTable[17]);  //42 is ENDSCENE
        DetourFunction(reinterpret_cast<std::uint8_t*>(o_DevicePresent), reinterpret_cast<std::uint8_t*>(&Device_Present), 5);
    }).detach();*/
}

void StartHook()
{
    InitialiseHooks();
}

/*void limit_render(std::function<void()> render, uint32_t max_frames)
{
	render();
	yield(std::chrono::milliseconds(1000 / max_frames));
}*/
#endif

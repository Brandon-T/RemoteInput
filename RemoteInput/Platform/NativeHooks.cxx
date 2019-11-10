#include "NativeHooks.hxx"
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined(__APPLE__)
#include "rd_route.h"
#endif

#include <memory>
#include <thread>
#include "JavaInternal.hxx"
#include "../EIOS.hxx"


#if defined(_WIN32) || defined(_WIN64)
typedef void (__stdcall *JavaNativeBlit_t)(JNIEnv *env, jobject joSelf, jobject srcData, jobject dstData, jobject clip, jint srcx, jint srcy, jint dstx, jint dsty, jint width, jint height, jint rmask, jint gmask, jint bmask, jboolean needLut);
JavaNativeBlit_t o_JavaNativeBlit;

#elif defined(__APPLE__)

typedef void (*JavaNativeBlit_t)(JNIEnv *env, void *oglc, jlong pSrcOps, jlong pDstOps, jboolean xform, jint hint, jint srctype, jboolean texture, jint sx1, jint sy1, jint sx2, jint sy2, jdouble dx1, jdouble dy1, jdouble dx2, jdouble dy2);
JavaNativeBlit_t o_JavaNativeBlit;

#else
typedef void (*JavaNativeBlit_t)(JNIEnv *env, jobject self, jobject srcData, jobject dstData, jobject comp, jobject clip, jint srcx, jint srcy, jint dstx, jint dsty, jint width, jint height);
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
				void *rasBase = reinterpret_cast<std::uint8_t*>(srcInfo.rasBase) + (srcInfo.scanStride * srcy) + (srcInfo.pixelStride * srcx);
				bool isRasterAligned = !(srcInfo.scanStride & 0x03);
				std::uint8_t* dest = control_center->get_image();

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

#elif defined(__APPLE__)

void JavaNativeBlit(JNIEnv *env, void *oglc, jlong pSrcOps, jlong pDstOps, jboolean xform, jint hint, jint srctype, jboolean texture, jint sx1, jint sy1, jint sx2, jint sy2, jdouble dx1, jdouble dy1, jdouble dx2, jdouble dy2)
{
	extern std::unique_ptr<ControlCenter> control_center;
	if (control_center)
	{
		jint width = sx2 - sx1;
		jint height = sy2 - sy1;
		SurfaceDataOps *srcOps = (SurfaceDataOps *)pSrcOps;

		if (!srcOps || width <= 0 || height <= 0)
		{
			return;
		}

		SurfaceDataRasInfo srcInfo = {0};
		srcInfo.bounds.x1 = sx1;
		srcInfo.bounds.y1 = sy1;
		srcInfo.bounds.x2 = sx2;
		srcInfo.bounds.y2 = sy2;

		if (srcOps->Lock(env, srcOps, &srcInfo, SD_LOCK_READ) == SD_SUCCESS)
		{
			srcOps->GetRasInfo(env, srcOps, &srcInfo);
			if (srcInfo.rasBase)
			{
				void *rasBase = reinterpret_cast<std::uint8_t*>(srcInfo.rasBase) + (srcInfo.scanStride * sy1) + (srcInfo.pixelStride * sx1);
				bool isRasterAligned = srcInfo.scanStride % srcInfo.pixelStride == 0; //!(srcInfo.scanStride & 0x03);
				std::uint8_t* dest = control_center->get_image();

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

	return o_JavaNativeBlit(env, oglc, pSrcOps, pDstOps, xform, hint, srctype, texture, sx1, sy1, sx2, sy2, dx1, dy1, dx2, dy2);
}

#else

Bool XShmPutImage(Display *display, Drawable d, GC gc, XImage *image, int src_x, int src_y, int dest_x, int dest_y, unsigned int width, unsigned int height, bool send_event)
{
	extern std::unique_ptr<ControlCenter> control_center;
	if (control_center)
	{
		int bytes_per_pixel = image->bits_per_pixel / 8;
		int stride = width * bytes_per_pixel;
		void *rasBase = reinterpret_cast<std::uint8_t*>(image->data) + (stride * src_y) + (bytes_per_pixel * src_x);
		std::uint8_t* dest = control_center->get_image();

		for (std::size_t i = 0; i < image->height; ++i)
		{
			for (std::size_t j = 0; j < image->width; ++j)
			{
				std::uint8_t pixel = image->data[i * image->width + (image->height - j - 1)];
				*dest++ = pixel;
			}
		}
	}

    typedef Bool (*XShmPutImage_t*)(Display*, Drawable, GC, XImage*, int, int, int, int, unsigned int, unsigned int, bool)
    static XShmPutImage_t o_XShmPutImage = reinterpret_cast<XShmPutImage_t>(dlsym(RTLD_NEXT, "XShmPutImage"));
    return o_XShmPutImage(display, d, gc, image, src_x, src_y, dest_x, dest_y, width, height, send_event);
}
#endif

void InitialiseHooks()
{
	#if defined(_WIN32) || defined(_WIN64)
    std::thread([]{
        while(!GetModuleHandle("awt.dll"))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        typedef int __stdcall (*MH_Initialize)(void);
        typedef int __stdcall (*MH_CreateHook)(LPVOID pTarget, LPVOID pDetour, LPVOID *ppOriginal);
        typedef int __stdcall (*MH_EnableHook)(LPVOID pTarget);

        HMODULE library = LoadLibrary("C:/Users/Brandon/Desktop/MinHook.x86.dll");
        MH_Initialize initialize = (MH_Initialize)GetProcAddress(library, "MH_Initialize");
        MH_CreateHook hook = (MH_CreateHook)GetProcAddress(library, "MH_CreateHook");
        MH_EnableHook enable = (MH_EnableHook)GetProcAddress(library, "MH_EnableHook");


        JavaNativeBlit_t original = (JavaNativeBlit_t)GetProcAddress(GetModuleHandle("awt.dll"), "_Java_sun_java2d_windows_GDIBlitLoops_nativeBlit@60");

        initialize();
        hook((void*)original, (void*)&JavaNativeBlit, (void**)&o_JavaNativeBlit);
        enable((void*)original);
    }).detach();
	#elif defined(__APPLE__)
	JavaNativeBlit_t blit = (JavaNativeBlit_t)dlsym(RTLD_NEXT, "OGLBlitLoops_Blit");
	rd_route((void*)blit, (void*)JavaNativeBlit, (void **)&o_JavaNativeBlit);
	#else
    #warning "FIX LINUX HOOKS"
	#endif
}

void StartHook()
{
    InitialiseHooks();
}

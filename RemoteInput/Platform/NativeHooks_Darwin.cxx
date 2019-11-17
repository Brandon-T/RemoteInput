#include "NativeHooks.hxx"
#if defined(__APPLE__)
#include "rd_route.h"

#include <memory>
#include <thread>
#include "JavaInternal.hxx"
#include "EIOS.hxx"
#include "Graphics.hxx"
#endif

#if defined(__APPLE__)
typedef void (*JavaNativeBlit_t)(JNIEnv *env, void *oglc, jlong pSrcOps, jlong pDstOps, jboolean xform, jint hint, jint srctype, jboolean texture, jint sx1, jint sy1, jint sx2, jint sy2, jdouble dx1, jdouble dy1, jdouble dx2, jdouble dy2);
JavaNativeBlit_t o_JavaNativeBlit;
#endif


#if defined(__APPLE__)
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
#endif

#if defined(__APPLE__)
void InitialiseHooks()
{
	JavaNativeBlit_t blit = (JavaNativeBlit_t)dlsym(RTLD_NEXT, "OGLBlitLoops_Blit");
	rd_route((void*)blit, (void*)JavaNativeBlit, (void **)&o_JavaNativeBlit);
}

void StartHook()
{
    InitialiseHooks();
}
#endif

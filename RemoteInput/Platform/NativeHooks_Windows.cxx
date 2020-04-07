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

#if __has_include("MinHook.h")
#include "MinHook.h"
#endif
#endif

#if defined(_WIN32) || defined(_WIN64)
std::unique_ptr<Hook> native_hook{nullptr};
std::unique_ptr<Hook> opengl_hook{nullptr};
std::unique_ptr<Hook> flush_buffer_hook{nullptr};

void __stdcall JavaNativeBlit(JNIEnv *env, jobject self, jobject srcData, jobject dstData, jobject comp, jobject clip, jint srcx, jint srcy, jint dstx, jint dsty, jint width, jint height)
{
    extern std::unique_ptr<ControlCenter> control_center;
    if (!control_center)
    {
        return native_hook->call<void, decltype(JavaNativeBlit)>(env, self, srcData, dstData, comp, clip, srcx, srcy, dstx, dsty, width, height);
    }

    #define Region_IsRectangular(pRgnInfo) ((pRgnInfo)->endIndex == 0)
    #define Region_IsEmpty(pRgnInfo) ((pRgnInfo)->bounds.x1 >= (pRgnInfo)->bounds.x2 || (pRgnInfo)->bounds.y1 >= (pRgnInfo)->bounds.y2)
    #define Region_IntersectBounds(pRgnInfo, pDstBounds) SurfaceData_IntersectBounds(&(pRgnInfo)->bounds, pDstBounds)

    #define PtrAddBytes(p, b)               ((void *) (((intptr_t) (p)) + (b)))
    #define PtrCoord(p, x, xinc, y, yinc)   PtrAddBytes(p, (y)*(yinc) + (x)*(xinc))

	static NativePrimitive* (*GetNativePrim)(JNIEnv *env, jobject gp) = reinterpret_cast<decltype(GetNativePrim)>(dlsym(RTLD_NEXT, "GetNativePrim"));
	static SurfaceDataOps* (*SurfaceData_GetOps)(JNIEnv *env, jobject sData) = reinterpret_cast<decltype(SurfaceData_GetOps)>(dlsym(RTLD_NEXT, "SurfaceData_GetOps"));
	static jint (*Region_GetInfo)(JNIEnv *env, jobject region, RegionData *pRgnInfo) = reinterpret_cast<decltype(Region_GetInfo)>(dlsym(RTLD_NEXT, "Region_GetInfo"));
    static void (*SurfaceData_IntersectBounds)(SurfaceDataBounds *src, SurfaceDataBounds *dst) = reinterpret_cast<decltype(SurfaceData_IntersectBounds)>(dlsym(RTLD_NEXT, "SurfaceData_IntersectBounds"));
    static void (*SurfaceData_IntersectBlitBounds)(SurfaceDataBounds *src, SurfaceDataBounds *dst, jint dx, jint dy) = reinterpret_cast<decltype(SurfaceData_IntersectBlitBounds)>(dlsym(RTLD_NEXT, "SurfaceData_IntersectBlitBounds"));
    static void (*Region_StartIteration)(JNIEnv *env, RegionData *pRgnInfo) = reinterpret_cast<decltype(Region_StartIteration)>(dlsym(RTLD_NEXT, "Region_StartIteration"));
    static jint (*Region_NextIteration)(RegionData *pRgnInfo, SurfaceDataBounds *pSpan) = reinterpret_cast<decltype(Region_NextIteration)>(dlsym(RTLD_NEXT, "Region_NextIteration"));
    static void (*Region_EndIteration)(JNIEnv *env, RegionData *pRgnInfo) = reinterpret_cast<decltype(Region_EndIteration)>(dlsym(RTLD_NEXT, "Region_EndIteration"));

	if (!GetNativePrim || !SurfaceData_GetOps || !Region_GetInfo || !SurfaceData_IntersectBounds || !SurfaceData_IntersectBlitBounds || !Region_StartIteration || !Region_NextIteration || !Region_EndIteration || width <= 0 || height <= 0)
    {
        return native_hook->call<void, decltype(JavaNativeBlit)>(env, self, srcData, dstData, comp, clip, srcx, srcy, dstx, dsty, width, height);
    }

    CompositeInfo compInfo = {0};
    NativePrimitive* pPrim = GetNativePrim(env, self);
    if (!pPrim)
    {
        return native_hook->call<void, decltype(JavaNativeBlit)>(env, self, srcData, dstData, comp, clip, srcx, srcy, dstx, dsty, width, height);
    }

    if (pPrim->pCompType->getCompInfo)
    {
        (*pPrim->pCompType->getCompInfo)(env, &compInfo, comp);
    }

    RegionData clipInfo = {0};
    if (Region_GetInfo(env, clip, &clipInfo))
    {
        return native_hook->call<void, decltype(JavaNativeBlit)>(env, self, srcData, dstData, comp, clip, srcx, srcy, dstx, dsty, width, height);
    }

    SurfaceDataOps *srcOps = SurfaceData_GetOps(env, srcData);
    SurfaceDataOps *dstOps = SurfaceData_GetOps(env, dstData);

    if (!srcOps || !dstOps)
    {
        return native_hook->call<void, decltype(JavaNativeBlit)>(env, self, srcData, dstData, comp, clip, srcx, srcy, dstx, dsty, width, height);
    }

    SurfaceDataRasInfo srcInfo = {0};
    SurfaceDataRasInfo dstInfo = {0};
    srcInfo.bounds.x1 = srcx;
    srcInfo.bounds.y1 = srcy;
    srcInfo.bounds.x2 = srcx + width;
    srcInfo.bounds.y2 = srcy + height;
    dstInfo.bounds.x1 = dstx;
    dstInfo.bounds.y1 = dsty;
    dstInfo.bounds.x2 = dstx + width;
    dstInfo.bounds.y2 = dsty + height;
    srcx -= dstx;
    srcy -= dsty;

    SurfaceData_IntersectBounds(&dstInfo.bounds, &clipInfo.bounds);

    if (srcOps->Lock(env, srcOps, &srcInfo, pPrim->srcflags) == SD_SUCCESS)
    {
        jint dstFlags = pPrim->dstflags;
        if (!Region_IsRectangular(&clipInfo))
        {
            dstFlags |= SD_LOCK_PARTIAL_WRITE;
        }

        if (dstOps->Lock(env, dstOps, &dstInfo, dstFlags) == SD_SUCCESS)
        {
            SurfaceData_IntersectBlitBounds(&dstInfo.bounds, &srcInfo.bounds, srcx, srcy);
            Region_IntersectBounds(&clipInfo, &dstInfo.bounds);

            if (!Region_IsEmpty(&clipInfo))
            {
                srcOps->GetRasInfo(env, srcOps, &srcInfo);
                dstOps->GetRasInfo(env, dstOps, &dstInfo);

                if (srcInfo.rasBase && dstInfo.rasBase)
                {
                    SurfaceDataBounds span;
                    jint savesx = srcInfo.bounds.x1;
                    jint savedx = dstInfo.bounds.x1;
                    Region_StartIteration(env, &clipInfo);

                    while (Region_NextIteration(&clipInfo, &span))
                    {
                        void *pSrc = PtrCoord(srcInfo.rasBase, srcx + span.x1, srcInfo.pixelStride, srcy + span.y1, srcInfo.scanStride);
                        void *pDst = PtrCoord(dstInfo.rasBase, span.x1, dstInfo.pixelStride, span.y1, dstInfo.scanStride);

                        srcInfo.bounds.x1 = srcx + span.x1;
                        dstInfo.bounds.x1 = span.x1;
                        (*pPrim->funcs.blit)(pSrc, pDst, span.x2 - span.x1, span.y2 - span.y1, &srcInfo, &dstInfo, pPrim, &compInfo);
                    }

                    Region_EndIteration(env, &clipInfo);
                    srcInfo.bounds.x1 = savesx;
                    dstInfo.bounds.x1 = savedx;

                    void* rasBase = reinterpret_cast<std::uint8_t*>(srcInfo.rasBase) + (srcInfo.scanStride * srcy) + (srcInfo.pixelStride * srcx);

                    control_center->update_dimensions(width, height);
                    std::uint8_t* dest = control_center->get_image();

                    //Render to Shared Memory
                    if (dest)
                    {
                        memcpy(dest, rasBase, (srcInfo.scanStride / srcInfo.pixelStride) * height * srcInfo.pixelStride);
                    }

                    //Render Debug Graphics
                    if (control_center->get_debug_graphics())
                    {
                        std::uint8_t* src = control_center->get_debug_image();
                        if (src)
                        {
                            rasBase = reinterpret_cast<std::uint8_t*>(dstInfo.rasBase) + (dstInfo.scanStride * dsty) + (dstInfo.pixelStride * dstx);
                            draw_image(rasBase, src, (dstInfo.scanStride / dstInfo.pixelStride), height, dstInfo.pixelStride);
                        }
                    }

                    //Render Cursor
                    std::int32_t x = -1;
                    std::int32_t y = -1;
                    control_center->get_applet_mouse_position(&x, &y);

                    if (x > -1 && y > -1)
                    {
                        rasBase = reinterpret_cast<std::uint8_t*>(dstInfo.rasBase) + (dstInfo.scanStride * dsty) + (dstInfo.pixelStride * dstx);
                        draw_circle(x, y, 2, rasBase, width, height, dstInfo.pixelStride, true, 0xFF0000FF);
                    }
                }
            }

            if (dstOps->Unlock)
            {
                dstOps->Unlock(env, dstOps, &dstInfo);
            }
        }

        if (srcOps->Unlock)
        {
            srcOps->Unlock(env, srcOps, &srcInfo);
        }
    }

    if (dstOps->Release)
    {
        dstOps->Release(env, dstOps, &dstInfo);
    }

    if (srcOps->Release)
    {
        srcOps->Release(env, srcOps, &srcInfo);
    }
}

void __stdcall JavaNativeOGLBlit(JNIEnv *env, void *oglc, jlong pSrcOps, jlong pDstOps, jboolean xform, jint hint, jint srctype, jboolean texture, jint sx1, jint sy1, jint sx2, jint sy2, jdouble dx1, jdouble dy1, jdouble dx2, jdouble dy2)
{
    extern std::unique_ptr<ControlCenter> control_center;
    if (control_center)
    {
        jint width = sx2 - sx1;
        jint height = sy2 - sy1;
        SurfaceDataOps *srcOps = (SurfaceDataOps *)pSrcOps;

        if (!srcOps || width <= 0 || height <= 0)
        {
            if (opengl_hook)
            {
                return opengl_hook->call<void, decltype(JavaNativeOGLBlit)>(env, oglc, pSrcOps, pDstOps, xform, hint, srctype, texture, sx1, sy1, sx2, sy2, dx1, dy1, dx2, dy2);
            }
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
                void* rasBase = reinterpret_cast<std::uint8_t*>(srcInfo.rasBase) + (srcInfo.scanStride * sy1) + (srcInfo.pixelStride * sx1);
                bool isRasterAligned = srcInfo.scanStride % srcInfo.pixelStride == 0; //!(srcInfo.scanStride & 0x03);

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
                        rasBase = reinterpret_cast<std::uint8_t*>(srcInfo.rasBase) + (srcInfo.scanStride * sy1) + (srcInfo.pixelStride * sx1);
                        draw_image(rasBase, src, (srcInfo.scanStride / srcInfo.pixelStride), height, srcInfo.pixelStride);
                    }
                }

                //Render Cursor
                std::int32_t x = -1;
                std::int32_t y = -1;
                control_center->get_applet_mouse_position(&x, &y);

                if (x > -1 && y > -1)
                {
                    rasBase = reinterpret_cast<std::uint8_t*>(srcInfo.rasBase) + (srcInfo.scanStride * sy1) + (srcInfo.pixelStride * sx1);
                    draw_circle(x, y, 2, rasBase, width, height, srcInfo.pixelStride, true, 0xFF0000FF);
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

    if (opengl_hook)
    {
        return opengl_hook->call<void, decltype(JavaNativeOGLBlit)>(env, oglc, pSrcOps, pDstOps, xform, hint, srctype, texture, sx1, sy1, sx2, sy2, dx1, dy1, dx2, dy2);
    }
}

void __stdcall JavaNativeOGLRenderQueueFlushBuffer(JNIEnv *env, jobject oglrq, jlong buf, jint limit)
{
    #define NEXT_VAL(buf, type) (((type *)((buf) += sizeof(type)))[-1])
    #define NEXT_INT(buf)       NEXT_VAL(buf, jint)
    #define NEXT_LONG(buf)      NEXT_VAL(buf, jlong)
    #define NEXT_DOUBLE(buf)    NEXT_VAL(buf, jdouble)

    #define EXTRACT_VAL(packedval, offset, mask) \
                (((packedval) >> (offset)) & (mask))
    #define EXTRACT_BYTE(packedval, offset) \
                (unsigned char)EXTRACT_VAL(packedval, offset, 0xff)
    #define EXTRACT_BOOLEAN(packedval, offset) \
                (jboolean)EXTRACT_VAL(packedval, offset, 0x1)

    const int BLIT                 = 31;
    const int OFFSET_SRCTYPE = 16;
    const int OFFSET_HINT    =  8;
    const int OFFSET_TEXTURE =  3;
    const int OFFSET_RTT     =  2;
    const int OFFSET_XFORM   =  1;
    const int OFFSET_ISOBLIT =  0;

    jlong original_buffer = buf;

    std::uint8_t* buffer = reinterpret_cast<uint8_t*>(buf);
    std::uint8_t* buffer_end = reinterpret_cast<uint8_t*>(buf) + limit;

    while(buffer < buffer_end)
    {
        jint opcode = NEXT_INT(buffer);
        switch (opcode)
        {
            case BLIT:
            {
                jint packedParams = NEXT_INT(buffer);
                jint sx1          = NEXT_INT(buffer);
                jint sy1          = NEXT_INT(buffer);
                jint sx2          = NEXT_INT(buffer);
                jint sy2          = NEXT_INT(buffer);
                jdouble dx1       = NEXT_DOUBLE(buffer);
                jdouble dy1       = NEXT_DOUBLE(buffer);
                jdouble dx2       = NEXT_DOUBLE(buffer);
                jdouble dy2       = NEXT_DOUBLE(buffer);
                jlong pSrc        = NEXT_LONG(buffer);
                jlong pDst        = NEXT_LONG(buffer);
                jint hint         = EXTRACT_BYTE(packedParams, OFFSET_HINT);
                jboolean texture  = EXTRACT_BOOLEAN(packedParams,
                                                    OFFSET_TEXTURE);
                jboolean rtt      = EXTRACT_BOOLEAN(packedParams,
                                                    OFFSET_RTT);
                jboolean xform    = EXTRACT_BOOLEAN(packedParams,
                                                    OFFSET_XFORM);
                jboolean isoblit  = EXTRACT_BOOLEAN(packedParams,
                                                    OFFSET_ISOBLIT);

                if (!isoblit)
                {
                    jint srctype = EXTRACT_BYTE(packedParams, OFFSET_SRCTYPE);
                    JavaNativeOGLBlit(env, nullptr, pSrc, pDst, xform, hint, srctype, texture, sx1, sy1, sx2, sy2, dx1, dy1, dx2, dy2);
                }
            }
                break;

            default:
                break;
        }
    }

    return flush_buffer_hook->call<void, decltype(JavaNativeOGLRenderQueueFlushBuffer)>(env, oglrq, original_buffer, limit);
}

void __stdcall JavaNativeGDIBlit(JNIEnv *env, jobject joSelf, jobject srcData, jobject dstData, jobject clip, jint srcx, jint srcy, jint dstx, jint dsty, jint width, jint height, jint rmask, jint gmask, jint bmask, jboolean needLut)
{
    extern std::unique_ptr<ControlCenter> control_center;
	if (!control_center)
	{
		//Original
		return native_hook->call<void, decltype(JavaNativeGDIBlit)>(env, joSelf, srcData, dstData, clip, srcx, srcy, dstx, dsty, width, height, rmask, gmask, bmask, needLut);
	}

	//Setup Function Pointers
    static HMODULE module = GetModuleHandle("awt.dll");
	static SurfaceDataOps* (__stdcall *SurfaceData_GetOps)(JNIEnv *env, jobject sData) = reinterpret_cast<SurfaceData_GetOps>(GetProcAddress(module, "_SurfaceData_GetOps@8") ?: GetProcAddress(module, "SurfaceData_GetOps"));
	static GDIWinSDOps* (__stdcall *GDIWindowSurfaceData_GetOps)(JNIEnv *env, jobject sData) = reinterpret_cast<GDIWindowSurfaceData_GetOps>(GetProcAddress(module, "_GDIWindowSurfaceData_GetOps@8") ?: GetProcAddress(module, "GDIWindowSurfaceData_GetOps"));
	static void (__stdcall *SurfaceData_IntersectBlitBounds)(SurfaceDataBounds *src, SurfaceDataBounds *dst, jint dx, jint dy) = reinterpret_cast<SurfaceData_IntersectBlitBounds>(GetProcAddress(module, "_SurfaceData_IntersectBlitBounds@16") ?: GetProcAddress(module, "SurfaceData_IntersectBlitBounds"));

	if (!SurfaceData_GetOps || !GDIWindowSurfaceData_GetOps || !SurfaceData_IntersectBlitBounds)
	{
		//Original
		return native_hook->call<void, decltype(JavaNativeGDIBlit)>(env, joSelf, srcData, dstData, clip, srcx, srcy, dstx, dsty, width, height, rmask, gmask, bmask, needLut);
	}

	//Setup Source Info
	SurfaceDataOps* srcOps = SurfaceData_GetOps(env, srcData);
	SurfaceDataRasInfo srcInfo = {0};
	srcInfo.bounds.x1 = srcx;
	srcInfo.bounds.y1 = srcy;
	srcInfo.bounds.x2 = srcx + width;
	srcInfo.bounds.y2 = srcy + height;

	if (srcOps->Lock(env, srcOps, &srcInfo, needLut ? (SD_LOCK_READ | SD_LOCK_LUT) : SD_LOCK_READ) == SD_SUCCESS)
	{
		//Setup Destination Clipping Info
		GDIWinSDOps* dstOps = GDIWindowSurfaceData_GetOps(env, dstData);
		SurfaceDataBounds dstBounds = {dstx, dsty, dstx + width, dsty + height};
		SurfaceData_IntersectBlitBounds(&(srcInfo.bounds), &dstBounds, dstx - srcx, dsty - srcy);
		srcx = srcInfo.bounds.x1;
		srcy = srcInfo.bounds.y1;
		dstx = dstBounds.x1;
		dsty = dstBounds.y1;
		width = srcInfo.bounds.x2 - srcInfo.bounds.x1;
		height = srcInfo.bounds.y2 - srcInfo.bounds.y1;

		if (width > 0 && height > 0)
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

				HDC hDC = dstOps->GetDC(env, dstOps, 0, nullptr, clip, nullptr, 0);
				if (hDC)
				{
					//Clone Renderer
					static jint screenWidth = 0;
					static jint screenHeight = 0;
					static std::unique_ptr<std::uint8_t[]> screenBuffer = nullptr;

					if (screenWidth != width || screenHeight != height)
					{
						screenBuffer.reset();
						screenWidth = width;
						screenHeight = height;
						screenBuffer = std::make_unique<std::uint8_t[]>(width * height * srcInfo.pixelStride);
					}

					rasBase = reinterpret_cast<std::uint8_t*>(srcInfo.rasBase) + (srcInfo.scanStride * srcy) + (srcInfo.pixelStride * srcx);
					if (isRasterAligned)
					{
						memcpy(screenBuffer.get(), rasBase, (srcInfo.scanStride / srcInfo.pixelStride) * height * srcInfo.pixelStride);
					}
					else
					{
						for (int i = 0; i < height; ++i)
						{
							int offset = (srcInfo.scanStride / srcInfo.pixelStride) * i;
							memcpy(screenBuffer.get() + offset, rasBase, (srcInfo.scanStride / srcInfo.pixelStride));
							rasBase = static_cast<void*>(reinterpret_cast<std::uint8_t*>(rasBase) + srcInfo.scanStride);
						}
					}

					//Render Debug Graphics
					if (control_center->get_debug_graphics())
					{
						std::uint8_t* src = control_center->get_debug_image();
						if (src)
						{
							rasBase = screenBuffer.get() + (srcInfo.scanStride * srcy) + (srcInfo.pixelStride * srcx);
							draw_image(rasBase, src, width, height, srcInfo.pixelStride);
						}
					}

					//Render Cursor
					std::int32_t x = -1;
					std::int32_t y = -1;
					control_center->get_applet_mouse_position(&x, &y);

					if (x > -1 && y > -1)
					{
						static const std::int32_t radius = 2;
						rasBase = screenBuffer.get() + (srcInfo.scanStride * srcy) + (srcInfo.pixelStride * srcx);
						draw_circle(x, y, radius, rasBase, width, height, 4, true, 0xFF0000FF);
					}

					//Render Screen
					BmiType bmi = {0};
					long dwHeight = srcInfo.bounds.y2 - srcInfo.bounds.y1;
					bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
					bmi.bmiHeader.biWidth = srcInfo.scanStride / srcInfo.pixelStride;
					bmi.bmiHeader.biHeight = isRasterAligned ? -dwHeight : -1;
					bmi.bmiHeader.biPlanes = 1;
					bmi.bmiHeader.biBitCount = srcInfo.pixelStride * 8;
					bmi.bmiHeader.biCompression = srcInfo.pixelStride != 2 ? BI_RGB : BI_BITFIELDS;
					bmi.bmiHeader.biSizeImage = (bmi.bmiHeader.biWidth * dwHeight * srcInfo.pixelStride);
					bmi.bmiHeader.biXPelsPerMeter = 0;
					bmi.bmiHeader.biYPelsPerMeter = 0;
					bmi.bmiHeader.biClrUsed = 0;
					bmi.bmiHeader.biClrImportant = 0;
					bmi.colors.dwMasks[0] = 0x00FF0000;
					bmi.colors.dwMasks[1] = 0x0000FF00;
					bmi.colors.dwMasks[2] = 0x000000FF;

					rasBase = screenBuffer.get() + (srcInfo.scanStride * srcy) + (srcInfo.pixelStride * srcx);

					if (isRasterAligned)
					{
						if (::IsWindowVisible(dstOps->window))
						{
							SetDIBitsToDevice(hDC, dstx, dsty, width, height, 0, 0, 0, height, rasBase, reinterpret_cast<BITMAPINFO*>(&bmi), DIB_RGB_COLORS);
						}
					}
					else
					{
						for (jint i = 0; i < height; i += 1)
						{
							if (::IsWindowVisible(dstOps->window))
							{
								SetDIBitsToDevice(hDC, dstx, dsty + i, width, 1, 0, 0, 0, 1, rasBase, reinterpret_cast<BITMAPINFO*>(&bmi), DIB_RGB_COLORS);
								rasBase = static_cast<void*>(reinterpret_cast<std::uint8_t*>(rasBase) + srcInfo.scanStride);
							}
							else
							{
								break;
							}
						}
					}

					dstOps->ReleaseDC(env, dstOps, hDC);
				}

				if (srcOps->Release)
				{
					srcOps->Release(env, srcOps, &srcInfo);
				}
			}
		}

		if (srcOps->Unlock)
		{
			srcOps->Unlock(env, srcOps, &srcInfo);
		}
	}
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
	if (w != width || h != height)
	{
	    w = width;
	    h = height;

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
		//memcpy(dest, data, width * height * 4);
		FlipImageBytes(data, (void*&)dest, width, height, 32);
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
			draw_circle(x, y, 3, rasBase, width, height, bytes_per_pixel, true, 0xFF0000FF);
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

		if (width >= 200 && height >= 200)
		{
			control_center->update_dimensions(width, height);

			//Render to Shared Memory
			std::uint8_t* dest = control_center->get_image();
			if (dest)
			{
				LoadOpenGLExtensions();
				GeneratePixelBuffers(hdc, pbo, width, height, 4);
				ReadPixelBuffers(hdc, dest, pbo, width, height, 4);
				//FlipImageVertically(width, height, dest);
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
    std::thread([&]{
		#if defined(USE_DETOURS)
		HMODULE module = GetModuleHandle("awt.dll");

		//Hook X11 Blit
		void* blit = GetProcAddress(module, "Java_sun_java2d_loops_Blit_Blit");
		if (blit)
		{
			native_hook = std::make_unique<Hook>(reinterpret_cast<void*>(blit), reinterpret_cast<void*>(JavaNativeBlit));
			native_hook->apply();
		}
		else
		{
			blit = GetProcAddress(module, "_Java_sun_java2d_windows_GDIBlitLoops_nativeBlit@60") ?: GetProcAddress(module, "Java_sun_java2d_windows_GDIBlitLoops_nativeBlit");
			if (blit)
			{
				native_hook = std::make_unique<Hook>(reinterpret_cast<void*>(blit), reinterpret_cast<void*>(JavaNativeGDIBlit));
				native_hook->apply();
			}
		}

		//Hook OpenGL Blit
		#if defined(HOOK_OPENGL_BLIT)
		blit = GetProcAddress(module, "OGLBlitLoops_Blit");
		if (blit)
		{
			opengl_hook = std::make_unique<Hook>(reinterpret_cast<void*>(blit), reinterpret_cast<void*>(JavaNativeOGLBlit));
			opengl_hook->apply();
		}
		else
		{
			blit = GetModuleHandle(module, "Java_sun_java2d_opengl_OGLRenderQueue_flushBuffer");
			if (blit)
			{
				flush_buffer_hook = std::make_unique<Hook>(reinterpret_cast<void*>(blit), reinterpret_cast<void*>(JavaNativeOGLRenderQueueFlushBuffer));
				flush_buffer_hook->apply();
			}
		}

		if (!blit)
		{
			module = GetModuleHandle("opengl32.dll");
			if (module)
			{
				#ifdef defined(HOOK_OPENGL_SWAP)
				blit = GetProcAddress(module, "wglSwapBuffers");
				opengl_hook = std::make_unique<Hook>(reinterpret_cast<void*>(blit), reinterpret_cast<void*>(mSwapBuffers));
				opengl_hook->apply();
				#else
				blit = GetProcAddress(module, "glDrawPixels");
				opengl_hook = std::make_unique<Hook>(reinterpret_cast<void*>(blit), reinterpret_cast<void*>(mglDrawPixels));
				opengl_hook->apply();
				#endif
			}
		}
		#endif
		#endif
    }).detach();

    /*std::thread([]{
		if (!GetModuleHandle("opengl32.dll"))
		{
			return;
		}

		decltype(o_glDrawPixels) original = (decltype(o_glDrawPixels))GetProcAddress(GetModuleHandle("opengl32.dll"), "glDrawPixels");

		MH_Initialize();
		MH_CreateHook(reinterpret_cast<void*>(original), reinterpret_cast<void*>(&mglDrawPixels), reinterpret_cast<void**>(&o_glDrawPixels));
		MH_EnableHook(reinterpret_cast<void*>(original));
    }).detach();*/

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

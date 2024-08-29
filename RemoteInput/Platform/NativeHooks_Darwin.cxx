#include "NativeHooks.hxx"
#if defined(__APPLE__)

#if defined(XCODE)
#include "main.hxx"
#else
#include "Thirdparty/Hook.hxx"
#endif

#include <OpenGL/OpenGL.h>
#include <OpenGL/CGLMacro.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>

#include <memory>
#include <thread>
#include "JavaInternal.hxx"
#include "EIOS.hxx"
#include "Graphics.hxx"
#endif

#if !defined(USE_DETOURS)
#define DYLD_INTERPOSE(_replacment,_replacee) \
__attribute__((used)) static struct{ const void* replacment; const void* replacee; } _interpose_##_replacee \
__attribute__ ((section ("__DATA,__interpose"))) = { (const void*)(unsigned long)&_replacment, (const void*)(unsigned long)&_replacee };
#endif

#if defined(__APPLE__)
std::unique_ptr<Hook> native_hook{nullptr};
std::unique_ptr<Hook> opengl_hook{nullptr};
std::unique_ptr<Hook> flush_buffer_hook{nullptr};

bool can_render(jint srctype, jint width, jint height)
{
    /* SRC_TYPE = OpenGLSurfaceData.PF_INT_RGBX = 3; //GL_RGBA, GL_UNSIGNED_INT_8_8_8_8
     * OGLPixelFormat pf = PixelFormats[srctype];
     *
     * It's used to render java.awt.canvas..
     * */
    if (srctype == 3 /* OpenGLSurfaceData.PF_INT_RGBX */)
    {
        return true;
    }

    //Arbitrarily chosen because the java.awt.canvas should never be smaller than this value.
    //Technically stuff should be rendering with a minimum of a power of 2.. so 256 * 256 for example.
    //This value can change if the renderer renders in 128 * 128 chunks but I haven't met a GPU that does that..
    //OR a Java implementation either.
    // - January 7th, 2021
    return width >= 200 && height >= 200;
}

void JavaNativeBlit(JNIEnv *env, jobject self, jobject srcData, jobject dstData, jobject comp, jobject clip, jint srcx, jint srcy, jint dstx, jint dsty, jint width, jint height) noexcept
{
    extern std::unique_ptr<ControlCenter> control_center;
    if (!control_center || !can_render(-1, width, height))
    {
        return native_hook->call<void, decltype(JavaNativeBlit)>(env, self, srcData, dstData, comp, clip, srcx, srcy, dstx, dsty, width, height);
    }

    #define Region_IsRectangular(pRgnInfo) ((pRgnInfo)->endIndex == 0)
    #define Region_IsEmpty(pRgnInfo) ((pRgnInfo)->bounds.x1 >= (pRgnInfo)->bounds.x2 || (pRgnInfo)->bounds.y1 >= (pRgnInfo)->bounds.y2)
    #define Region_IntersectBounds(pRgnInfo, pDstBounds) SurfaceData_IntersectBounds(&(pRgnInfo)->bounds, pDstBounds)

    #define PtrAddBytes(p, b)               ((void *) (((intptr_t) (p)) + (b)))
    #define PtrCoord(p, x, xinc, y, yinc)   PtrAddBytes(p, (y)*(yinc) + (x)*(xinc))

    static NativePrimitive* (*GetNativePrim)(JNIEnv *env, jobject gp) = reinterpret_cast<decltype(GetNativePrim)>(dlsym(RTLD_DEFAULT, "GetNativePrim"));
    static SurfaceDataOps* (*SurfaceData_GetOps)(JNIEnv *env, jobject sData) = reinterpret_cast<decltype(SurfaceData_GetOps)>(dlsym(RTLD_DEFAULT, "SurfaceData_GetOps"));
    static jint (*Region_GetInfo)(JNIEnv *env, jobject region, RegionData *pRgnInfo) = reinterpret_cast<decltype(Region_GetInfo)>(dlsym(RTLD_DEFAULT, "Region_GetInfo"));
    static void (*SurfaceData_IntersectBounds)(SurfaceDataBounds *src, SurfaceDataBounds *dst) = reinterpret_cast<decltype(SurfaceData_IntersectBounds)>(dlsym(RTLD_DEFAULT, "SurfaceData_IntersectBounds"));
    static void (*SurfaceData_IntersectBlitBounds)(SurfaceDataBounds *src, SurfaceDataBounds *dst, jint dx, jint dy) = reinterpret_cast<decltype(SurfaceData_IntersectBlitBounds)>(dlsym(RTLD_NEXT, "SurfaceData_IntersectBlitBounds"));
    static void (*Region_StartIteration)(JNIEnv *env, RegionData *pRgnInfo) = reinterpret_cast<decltype(Region_StartIteration)>(dlsym(RTLD_DEFAULT, "Region_StartIteration"));
    static jint (*Region_NextIteration)(RegionData *pRgnInfo, SurfaceDataBounds *pSpan) = reinterpret_cast<decltype(Region_NextIteration)>(dlsym(RTLD_DEFAULT, "Region_NextIteration"));
    static void (*Region_EndIteration)(JNIEnv *env, RegionData *pRgnInfo) = reinterpret_cast<decltype(Region_EndIteration)>(dlsym(RTLD_DEFAULT, "Region_EndIteration"));

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

                    control_center->set_target_dimensions(width, height);
                    std::uint8_t* dest = control_center->get_image();
                    ImageFormat format = control_center->get_image_format();

                    //Render to Shared Memory
                    if (dest)
                    {
                        copy_image(dest, rasBase, (srcInfo.scanStride / srcInfo.pixelStride), height, srcInfo.pixelStride, format);
                    }

                    //Render Debug Graphics
                    if (control_center->get_debug_graphics())
                    {
                        std::uint8_t* src = control_center->get_debug_image();
                        if (src)
                        {
                            rasBase = reinterpret_cast<std::uint8_t*>(dstInfo.rasBase) + (dstInfo.scanStride * dsty) + (dstInfo.pixelStride * dstx);
                            draw_image(rasBase, src, (dstInfo.scanStride / dstInfo.pixelStride), height, dstInfo.pixelStride, format);
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

void JavaNativeOGLBlit(JNIEnv *env, void *oglc, jlong pSrcOps, jlong pDstOps, jboolean xform, jint hint, jint srctype, jboolean texture, jint sx1, jint sy1, jint sx2, jint sy2, jdouble dx1, jdouble dy1, jdouble dx2, jdouble dy2) noexcept
{
    extern std::unique_ptr<ControlCenter> control_center;
    if (control_center)
    {
        jint width = sx2 - sx1;
        jint height = sy2 - sy1;
        SurfaceDataOps *srcOps = (SurfaceDataOps *)pSrcOps;

        if (!srcOps || width <= 0 || height <= 0 || !can_render(srctype, width, height))
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

                control_center->set_target_dimensions(width, height);
                std::uint8_t* dest = control_center->get_image();
                ImageFormat format = control_center->get_image_format();

                //Render to Shared Memory
                if (dest)
                {
                    if (isRasterAligned)
                    {
                        copy_image(dest, rasBase, (srcInfo.scanStride / srcInfo.pixelStride), height, srcInfo.pixelStride, format);
                    }
                    else
                    {
                        for (int i = 0; i < height; ++i)
                        {
                            int offset = (srcInfo.scanStride / srcInfo.pixelStride) * i;
                            copy_image(dest + offset, rasBase, (srcInfo.scanStride / srcInfo.pixelStride), 1, srcInfo.pixelStride, format);
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
                        draw_image(rasBase, src, (srcInfo.scanStride / srcInfo.pixelStride), height, srcInfo.pixelStride, format);
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

void JavaNativeOGLRenderQueueFlushBuffer(JNIEnv *env, jobject oglrq, jlong buf, jint limit) noexcept
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

    const int BLIT           = 31;
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
#endif

#if defined(__APPLE__)
void GeneratePixelBuffers(void* ctx, GLuint (&pbo)[2], GLint width, GLint height, GLint stride) noexcept
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

void ReadPixelBuffers(void* ctx, GLubyte* dest, GLuint (&pbo)[2], GLint width, GLint height, GLint stride, ImageFormat format) noexcept
{
    static int index = 0;
    static int nextIndex = 0;

    #if defined(__APPLE__)
    CGLContextObj CGL_MACRO_CONTEXT = static_cast<CGLContextObj>(ctx);
    #endif

    //Swap indices
    index = (index + 1) % 2;
    nextIndex = (index + 1) % 2;

    GLenum gl_format = [](ImageFormat format) -> GLenum {
        switch(format)
        {
            case ImageFormat::BGR_BGRA: return GL_BGRA;
            case ImageFormat::BGRA: return GL_BGRA;
            case ImageFormat::RGBA: return GL_RGBA;
            case ImageFormat::ARGB: return 0;  // Not Supported
            case ImageFormat::ABGR: return 0;  // Not Supported
        }
    }(format);

    //Read back-buffer.
    glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo[index]);
    glReadPixels(0, 0, width, height, gl_format, GL_UNSIGNED_BYTE, nullptr);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo[nextIndex]);

    void* data = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

    if (data)
    {
        //memcpy(dest, data, width * height * 4);
        FlipImageBytes(data, reinterpret_cast<void*&>(dest), width, height, 32);
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    }
    else
    {
        glReadPixels(0, 0, width, height, gl_format, GL_UNSIGNED_BYTE, dest);
    }

    //Restore state
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
}

#if defined(USE_DETOURS)
CGLError mCGLFlushDrawable(CGLContextObj ctx) noexcept
{
    extern std::unique_ptr<ControlCenter> control_center;

    if (control_center)
    {
        static GLint ViewPort[4] = {0};
        static GLuint pbo[2] = {0};
        CGLContextObj CGL_MACRO_CONTEXT = ctx;

        glGetIntegerv(GL_VIEWPORT, ViewPort);
        GLint width = ViewPort[2] - ViewPort[0];
        GLint height = ViewPort[3] - ViewPort[1];

        if (can_render(-1, width, height))
        {
            control_center->set_target_dimensions(width, height);

            //Render to Shared Memory
            std::uint8_t* dest = control_center->get_image();
            ImageFormat format = control_center->get_image_format();
            if (dest)
            {
                GeneratePixelBuffers(ctx, pbo, width, height, 4);
                ReadPixelBuffers(ctx, dest, pbo, width, height, 4, format);
                //FlipImageVertically(width, height, dest);
            }

            //Render Debug Graphics
            if (control_center->get_debug_graphics())
            {
                std::uint8_t* src = control_center->get_debug_image();
                if (src)
                {
                    gl_draw_image(ctx, src, 0, 0, width, height, 4, format);
                }
            }

            //Render Cursor
            std::int32_t x = -1;
            std::int32_t y = -1;
            control_center->get_applet_mouse_position(&x, &y);

            if (x > -1 && y > -1)
            {
                glColor4ub(0xFF, 0x00, 0x00, 0xFF);
                gl_draw_point(ctx, x, height - y, 0, 4);
            }
        }
    }

    return opengl_hook->call<CGLError, decltype(mCGLFlushDrawable)>(ctx);
}
#else
CGLError mCGLFlushDrawable(CGLContextObj ctx) noexcept
{
    extern std::unique_ptr<ControlCenter> control_center;

    if (control_center)
    {
        static GLint ViewPort[4] = {0};
        static GLuint pbo[2] = {0};
        CGLContextObj CGL_MACRO_CONTEXT = ctx;

        glGetIntegerv(GL_VIEWPORT, ViewPort);
        GLint width = ViewPort[2] - ViewPort[0];
        GLint height = ViewPort[3] - ViewPort[1];

        if (can_render(-1, width, height))
        {
            control_center->set_target_dimensions(width, height);

            //Render to Shared Memory
            std::uint8_t* dest = control_center->get_image();
            ImageFormat format = control_center->get_image_format();
            if (dest)
            {
                GeneratePixelBuffers(ctx, pbo, width, height, 4);
                ReadPixelBuffers(ctx, dest, pbo, width, height, 4, format);
                //FlipImageVertically(width, height, dest);
            }

            //Render Debug Graphics
            if (control_center->get_debug_graphics())
            {
                std::uint8_t* src = control_center->get_debug_image();
                if (src)
                {
                    gl_draw_image(ctx, src, 0, 0, width, height, 4, format);
                }
            }

            //Render Cursor
            std::int32_t x = -1;
            std::int32_t y = -1;
            control_center->get_applet_mouse_position(&x, &y);

            if (x > -1 && y > -1)
            {
                glColor4ub(0xFF, 0x00, 0x00, 0xFF);
                gl_draw_point(ctx, x, height - y, 0, 4);
            }
        }
    }

    static decltype(CGLFlushDrawable)* o_CGLFlushDrawable = reinterpret_cast<decltype(CGLFlushDrawable)*>(dlsym(RTLD_NEXT, "CGLFlushDrawable"));
    return o_CGLFlushDrawable(ctx);
}
#endif
#endif

#if defined(__APPLE__)
void InitialiseHooks() noexcept
{
    #if defined(USE_DETOURS)
        //Hook Native Blit
        void* blit = dlsym(RTLD_DEFAULT, "Java_sun_java2d_loops_Blit_Blit");
        if (blit)
        {
            native_hook = std::make_unique<Hook>(reinterpret_cast<void*>(blit), reinterpret_cast<void*>(JavaNativeBlit));
            native_hook->apply();
        }

        //Hook OpenGL Blit
        #if defined(HOOK_OPENGL_BLIT)
        blit = dlsym(RTLD_DEFAULT, "OGLBlitLoops_Blit");
        if (blit)
        {
            opengl_hook = std::make_unique<Hook>(reinterpret_cast<void*>(blit), reinterpret_cast<void*>(JavaNativeOGLBlit));
            opengl_hook->apply();
        }
        else
        {
            blit = dlsym(RTLD_DEFAULT, "Java_sun_java2d_opengl_OGLRenderQueue_flushBuffer");
            if (blit)
            {
                flush_buffer_hook = std::make_unique<Hook>(reinterpret_cast<void*>(blit), reinterpret_cast<void*>(JavaNativeOGLRenderQueueFlushBuffer));
                flush_buffer_hook->apply();
            }
        }

        if (!blit)
        {
            blit = dlsym(RTLD_DEFAULT, "CGLFlushDrawable");
            opengl_hook = std::make_unique<Hook>(reinterpret_cast<void*>(blit), reinterpret_cast<void*>(mCGLFlushDrawable));
            opengl_hook->apply();
        }
        #endif
    #else
        DYLD_INTERPOSE(mCGLFlushDrawable, CGLFlushDrawable);
    #endif
}

void StartHook() noexcept
{
    InitialiseHooks();

    //Signal that all hooks are finished initializing..
    ControlCenter::signal_sync(getpid());
}
#endif
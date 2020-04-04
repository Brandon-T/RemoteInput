#include "NativeHooks.hxx"
#if defined(__linux__)
#include <memory>
#include <thread>
#include "JavaInternal.hxx"
#include "EIOS.hxx"
#include "Graphics.hxx"

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glx.h>
#endif


#if defined(__linux__)
typedef void (*JavaNativeBlit_t)(JNIEnv *env, jobject self, jobject srcData, jobject dstData, jobject comp, jobject clip, jint srcx, jint srcy, jint dstx, jint dsty, jint width, jint height);
JavaNativeBlit_t o_JavaNativeBlit;

extern "C" [[gnu::visibility("default")]] void Java_sun_java2d_loops_Blit_Blit(JNIEnv *env, jobject self, jobject srcData, jobject dstData, jobject comp, jobject clip, jint srcx, jint srcy, jint dstx, jint dsty, jint width, jint height)
{
    extern std::unique_ptr<ControlCenter> control_center;
    if (!control_center)
    {
        return o_JavaNativeBlit(env, self, srcData, dstData, comp, clip, srcx, srcy, dstx, dsty, width, height);
    }

    #define Region_IsRectangular(pRgnInfo) ((pRgnInfo)->endIndex == 0)
    #define Region_IsEmpty(pRgnInfo) ((pRgnInfo)->bounds.x1 >= (pRgnInfo)->bounds.x2 || (pRgnInfo)->bounds.y1 >= (pRgnInfo)->bounds.y2)
    #define Region_IntersectBounds(pRgnInfo, pDstBounds) IntersectBounds(&(pRgnInfo)->bounds, pDstBounds)

    #define PtrAddBytes(p, b)               ((void *) (((intptr_t) (p)) + (b)))
    #define PtrCoord(p, x, xinc, y, yinc)   PtrAddBytes(p, (y)*(yinc) + (x)*(xinc))

    typedef NativePrimitive* (*GetNativePrim)(JNIEnv *env, jobject gp);
	static GetNativePrim GetPrimitive = (GetNativePrim)dlsym(RTLD_NEXT, "GetNativePrim");

	typedef SurfaceDataOps* (*SurfaceData_GetOps)(JNIEnv *env, jobject sData);
	static SurfaceData_GetOps GetOps = (SurfaceData_GetOps)dlsym(RTLD_NEXT, "SurfaceData_GetOps");

	typedef jint (*Region_GetInfo)(JNIEnv *env, jobject region, RegionData *pRgnInfo);
	static Region_GetInfo GetRegionInfo = (Region_GetInfo)dlsym(RTLD_NEXT, "Region_GetInfo");

	typedef void (*SurfaceData_IntersectBounds)(SurfaceDataBounds *src, SurfaceDataBounds *dst);
	static SurfaceData_IntersectBounds IntersectBounds = (SurfaceData_IntersectBounds)dlsym(RTLD_NEXT, "SurfaceData_IntersectBounds");

	typedef void (*SurfaceData_IntersectBlitBounds)(SurfaceDataBounds *src, SurfaceDataBounds *dst, jint dx, jint dy);
	static SurfaceData_IntersectBlitBounds IntersectBlitBounds = (SurfaceData_IntersectBlitBounds)dlsym(RTLD_NEXT, "SurfaceData_IntersectBlitBounds");

	typedef void (*Region_StartIteration)(JNIEnv *env, RegionData *pRgnInfo);
	static Region_StartIteration StartIteration = (Region_StartIteration)dlsym(RTLD_NEXT, "Region_StartIteration");

	typedef jint (*Region_NextIteration)(RegionData *pRgnInfo, SurfaceDataBounds *pSpan);
	static Region_NextIteration NextIteration = (Region_NextIteration)dlsym(RTLD_NEXT, "Region_NextIteration");

	typedef void (*Region_EndIteration)(JNIEnv *env, RegionData *pRgnInfo);
	static Region_EndIteration EndIteration = (Region_EndIteration)dlsym(RTLD_NEXT, "Region_EndIteration");


	if (!GetPrimitive || !GetOps || !GetRegionInfo || !IntersectBounds || width <= 0 || height <= 0)
    {
        return o_JavaNativeBlit(env, self, srcData, dstData, comp, clip, srcx, srcy, dstx, dsty, width, height);
    }

    CompositeInfo compInfo = {0};
    NativePrimitive* pPrim = GetPrimitive(env, self);
    if (!pPrim)
    {
        return o_JavaNativeBlit(env, self, srcData, dstData, comp, clip, srcx, srcy, dstx, dsty, width, height);
    }

    if (pPrim->pCompType->getCompInfo)
    {
        (*pPrim->pCompType->getCompInfo)(env, &compInfo, comp);
    }

    RegionData clipInfo = {0};
    if (GetRegionInfo(env, clip, &clipInfo))
    {
        return o_JavaNativeBlit(env, self, srcData, dstData, comp, clip, srcx, srcy, dstx, dsty, width, height);
    }

    SurfaceDataOps *srcOps = GetOps(env, srcData);
    SurfaceDataOps *dstOps = GetOps(env, dstData);

    if (!srcOps || !dstOps)
    {
        return o_JavaNativeBlit(env, self, srcData, dstData, comp, clip, srcx, srcy, dstx, dsty, width, height);
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

    IntersectBounds(&dstInfo.bounds, &clipInfo.bounds);

    jint dstFlags = pPrim->dstflags;
    if (!Region_IsRectangular(&clipInfo))
    {
        dstFlags |= SD_LOCK_PARTIAL_WRITE;
    }

    if (dstOps->Lock(env, srcOps, &srcInfo, SD_LOCK_READ) == SD_SUCCESS)
    {
        IntersectBlitBounds(&dstInfo.bounds, &srcInfo.bounds, srcx, srcy);
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
                StartIteration(env, &clipInfo);

                while (NextIteration(&clipInfo, &span))
                {
                    void *pSrc = PtrCoord(srcInfo.rasBase, srcx + span.x1, srcInfo.pixelStride, srcy + span.y1, srcInfo.scanStride);
                    void *pDst = PtrCoord(dstInfo.rasBase, span.x1, dstInfo.pixelStride, span.y1, dstInfo.scanStride);

                    srcInfo.bounds.x1 = srcx + span.x1;
                    dstInfo.bounds.x1 = span.x1;
                    (*pPrim->funcs.blit)(pSrc, pDst, span.x2 - span.x1, span.y2 - span.y1, &srcInfo, &dstInfo, pPrim, &compInfo);
                }

                EndIteration(env, &clipInfo);
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

            if (dstOps->Release)
            {
                dstOps->Release(env, dstOps, &dstInfo);
            }

            if (srcOps->Release)
            {
                srcOps->Release(env, srcOps, &srcInfo);
            }
        }

        if (dstOps->Unlock)
        {
            dstOps->Unlock(env, dstOps, &dstInfo);
        }
    }
}
#endif

#if defined(__linux__)
extern "C" [[gnu::visibility("default")]] Bool XShmPutImage(Display *display, Drawable d, GC gc, XImage *image, int src_x, int src_y, int dest_x, int dest_y, unsigned int width, unsigned int height, bool send_event)
{
	extern std::unique_ptr<ControlCenter> control_center;
	if (control_center)
	{
		int bytes_per_pixel = image->bits_per_pixel / 8;
		int stride = width * bytes_per_pixel;
		void *rasBase = reinterpret_cast<std::uint8_t*>(image->data) + (stride * src_y) + (bytes_per_pixel * src_x);

		control_center->update_dimensions(width, height);

		//Render to Shared Memory
		std::uint8_t* dest = control_center->get_image();
		if (dest)
		{
			std::memcpy(dest, rasBase, (stride / bytes_per_pixel) * height * bytes_per_pixel);
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
			draw_circle(x, y, 2, rasBase, width, height, bytes_per_pixel, true, 0xFF0000FF);
		}

		/*for (std::size_t i = 0; i < image->height; ++i)
		{
			for (std::size_t j = 0; j < image->width; ++j)
			{
				std::uint8_t pixel = image->data[i * image->width + (image->height - j - 1)];
				*dest++ = pixel;
			}
		}*/
	}

    typedef Bool (*XShmPutImage_t)(Display*, Drawable, GC, XImage*, int, int, int, int, unsigned int, unsigned int, bool);
    static XShmPutImage_t o_XShmPutImage = reinterpret_cast<XShmPutImage_t>(dlsym(RTLD_NEXT, "XShmPutImage"));
    return o_XShmPutImage(display, d, gc, image, src_x, src_y, dest_x, dest_y, width, height, send_event);
}
#endif

#if defined(__linux__)
void GeneratePixelBuffers(GLXDrawable ctx, GLuint (&pbo)[2], GLint width, GLint height, GLint stride)
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

void ReadPixelBuffers(GLXDrawable ctx, GLubyte* dest, GLuint (&pbo)[2], GLint width, GLint height, GLint stride)
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

extern "C" [[gnu::visibility("default")]] void glXSwapBuffers(Display* dpy, GLXDrawable drawable)
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
				GeneratePixelBuffers(drawable, pbo, width, height, 4);
				ReadPixelBuffers(drawable, dest, pbo, width, height, 4);
				//FlipImageVertically(width, height, dest);
			}

			//Render Debug Graphics
			if (control_center->get_debug_graphics())
			{
				std::uint8_t* src = control_center->get_debug_image();
				if (src)
				{
					gl_draw_image(reinterpret_cast<void*>(drawable), src, 0, 0, width, height, 4);
				}
			}

			//Render Cursor
			std::int32_t x = -1;
			std::int32_t y = -1;
			control_center->get_applet_mouse_position(&x, &y);

			if (x > -1 && y > -1)
			{
				glColor4ub(0xFF, 0x00, 0x00, 0xFF);
				gl_draw_point(reinterpret_cast<void*>(drawable), x, y, 0, 4);
			}
		}
	}

	static decltype(glXSwapBuffers)* o_glXSwapBuffers = reinterpret_cast<decltype(glXSwapBuffers)*>(dlsym(RTLD_NEXT, "glXSwapBuffers"));
	return o_glXSwapBuffers(dpy, drawable);
}
#endif

#if defined(__linux__)
void InitialiseHooks()
{
    #warning "FIX LINUX HOOKS"
    //TODO: Fix via Detours.. but should work otherwise
}

void StartHook()
{
    InitialiseHooks();
}
#endif

#include "NativeHooks.hxx"
#if defined(__APPLE__)

#if __has_include("rd_route.h")
#include "rd_route.h"
#else
#include <OpenGL/OpenGL.h>
#include <OpenGL/CGLMacro.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#endif

#include <memory>
#include <thread>
#include "JavaInternal.hxx"
#include "EIOS.hxx"
#include "Graphics.hxx"
#endif

#if !__has_include("rd_route.h")
#define DYLD_INTERPOSE(_replacment,_replacee) \
__attribute__((used)) static struct{ const void* replacment; const void* replacee; } _interpose_##_replacee \
__attribute__ ((section ("__DATA,__interpose"))) = { (const void*)(unsigned long)&_replacment, (const void*)(unsigned long)&_replacee };
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

#if defined(__APPLE__) && !__has_include("rd_route.h")
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

#if __has_include("rd_route.h")
CGLError CGLFlushDrawable(CGLContextObj ctx)
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
		
		
		std::uint8_t* dest = control_center->get_image();
		GeneratePixelBuffers(ctx, pbo, width, height, 4);
		ReadPixelBuffers(ctx, dest, pbo, width, height, 4);
	}
	
	static decltype(CGLFlushDrawable)* o_CGLFlushDrawable = reinterpret_cast<decltype(CGLFlushDrawable)*>(dlsym(RTLD_NEXT, "CGLFlushDrawable"));
	return o_CGLFlushDrawable(ctx);
}
#else
CGLError mCGLFlushDrawable(CGLContextObj ctx)
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
		
		
		std::uint8_t* dest = control_center->get_image();
		GeneratePixelBuffers(ctx, pbo, width, height, 4);
		ReadPixelBuffers(ctx, dest, pbo, width, height, 4);
		
		gl_draw_image(ctx, control_center->get_debug_image(), width, height, 4);
	}
	
	static decltype(CGLFlushDrawable)* o_CGLFlushDrawable = reinterpret_cast<decltype(CGLFlushDrawable)*>(dlsym(RTLD_NEXT, "CGLFlushDrawable"));
	return o_CGLFlushDrawable(ctx);
}
#endif
#endif

#if defined(__APPLE__)
void InitialiseHooks()
{
	#if __has_include("rd_route.h")
	JavaNativeBlit_t blit = (JavaNativeBlit_t)dlsym(RTLD_NEXT, "OGLBlitLoops_Blit");
	if (blit)
	{
		rd_route((void*)blit, (void*)JavaNativeBlit, (void **)&o_JavaNativeBlit);
	}
	#else
	DYLD_INTERPOSE(mCGLFlushDrawable, CGLFlushDrawable);
	#endif
}

void StartHook()
{
    InitialiseHooks();
}
#endif

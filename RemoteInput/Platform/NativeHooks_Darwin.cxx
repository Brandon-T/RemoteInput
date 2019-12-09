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
#include "ModelRendering.hpp"


void DrawLine(CGLContextObj ctx, jint x1, jint y1, jint x2, jint y2)
{
	CGLContextObj CGL_MACRO_CONTEXT = ctx;
	
    glBegin(GL_LINES);
    if (y1 == y2) {
        GLfloat fx1 = (GLfloat)x1;
        GLfloat fx2 = (GLfloat)x2;
        GLfloat fy  = ((GLfloat)y1) + 0.2f;

        if (x1 > x2) {
            GLfloat t = fx1; fx1 = fx2; fx2 = t;
        }

        glVertex2f(fx1+0.2f, fy);
        glVertex2f(fx2+1.2f, fy);
    } else if (x1 == x2) {
        GLfloat fx  = ((GLfloat)x1) + 0.2f;
        GLfloat fy1 = (GLfloat)y1;
        GLfloat fy2 = (GLfloat)y2;

        if (y1 > y2) {
            GLfloat t = fy1; fy1 = fy2; fy2 = t;
        }

        glVertex2f(fx, fy1+0.2f);
        glVertex2f(fx, fy2+1.2f);
    } else {
        GLfloat fx1 = (GLfloat)x1;
        GLfloat fy1 = (GLfloat)y1;
        GLfloat fx2 = (GLfloat)x2;
        GLfloat fy2 = (GLfloat)y2;

        if (x1 < x2) {
            fx1 += 0.2f;
            fx2 += 1.0f;
        } else {
            fx1 += 0.8f;
            fx2 -= 0.2f;
        }

        if (y1 < y2) {
            fy1 += 0.2f;
            fy2 += 1.0f;
        } else {
            fy1 += 0.8f;
            fy2 -= 0.2f;
        }

        glVertex2f(fx1, fy1);
        glVertex2f(fx2, fy2);
    }
	glEnd();
}

void DrawPoly(CGLContextObj ctx,
			  jint nPoints, jint isClosed,
			  jint transX, jint transY,
			  jint *xPoints, jint *yPoints)
{
	CGLContextObj CGL_MACRO_CONTEXT = ctx;
    jboolean isEmpty = JNI_TRUE;
    jint mx, my;
    jint i;

    if (xPoints == NULL || yPoints == NULL)
	{
        return;
    }

    mx = xPoints[0];
    my = yPoints[0];

	glBegin(GL_LINE_STRIP);
    for (i = 0; i < nPoints; i++) {
        jint x = xPoints[i];
        jint y = yPoints[i];

        isEmpty = isEmpty && (x == mx && y == my);
        glVertex2f((GLfloat)(x + transX) + 0.5f,
				   (GLfloat)(y + transY) + 0.5f);
    }

    if (isClosed && !isEmpty &&
        (xPoints[nPoints-1] != mx ||
         yPoints[nPoints-1] != my))
    {
        glVertex2f((GLfloat)(mx + transX) + 0.5f,
				   (GLfloat)(my + transY) + 0.5f);
		glEnd();
    } else if (!isClosed || isEmpty) {
		glBegin(GL_LINES);
        mx = xPoints[nPoints-1] + transX;
        my = yPoints[nPoints-1] + transY;
        glVertex2i(mx, my);
        glVertex2i(mx+1, my+1);
    } else {
		glEnd();
    }
}

void DrawPolyLines(CGLContextObj ctx, std::int32_t npoints, std::int32_t* xpoints, std::int32_t* ypoints)
{
	std::int32_t size = npoints - 2;
	for (int i = 0; i <= size; ++i)
	{
		DrawLine(ctx, xpoints[i], ypoints[i], xpoints[i + 1], ypoints[i + 1]);
	}
	
	DrawLine(ctx, xpoints[size + 1], ypoints[size + 1], xpoints[0], ypoints[0]);
}




void DrawLine2(int x, int y, int x2, int y2, std::function<void(int x, int y)> plot)
{
	bool yLonger=false;
	int incrementVal;
	int shortLen=y2-y;
	int longLen=x2-x;

	if (abs(shortLen)>abs(longLen))
	{
		int swap=shortLen;
		shortLen=longLen;
		longLen=swap;
		yLonger=true;
	}

	if (longLen<0) incrementVal=-1;
	else incrementVal=1;

	double multDiff;
	if (longLen==0.0) multDiff=(double)shortLen;
	else multDiff=(double)shortLen/(double)longLen;
	if (yLonger)
	{
		for (int i=0;i!=longLen;i+=incrementVal)
		{
			plot(x+(int)((double)i*multDiff),y+i);
		}
	}
	else
	{
		for (int i=0;i!=longLen;i+=incrementVal)
		{
			plot(x+i,y+(int)((double)i*multDiff));
		}
	}
}

void DrawPolyLines2(std::int32_t npoints, std::int32_t* xpoints, std::int32_t* ypoints, std::function<void(int x, int y)> plot)
{
	std::int32_t size = npoints - 2;
	for (int i = 0; i <= size; ++i)
	{
		DrawLine2(xpoints[i], ypoints[i], xpoints[i + 1], ypoints[i + 1], plot);
	}
	
	DrawLine2(xpoints[size + 1], ypoints[size + 1], xpoints[0], ypoints[0], plot);
}

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
		
		control_center->update_dimensions(width, height);
		
		std::uint8_t* dest = control_center->get_image();
		GeneratePixelBuffers(ctx, pbo, width, height, 4);
		ReadPixelBuffers(ctx, dest, pbo, width, height, 4);
		
		
		std::uint8_t* src = control_center->get_debug_image();
		//draw_circle(200, 200, 50, src, width, height, 4, true);
		gl_draw_image(ctx, src, width, height, 4);
		
		/*glPushMatrix();
		glLoadIdentity();
		draw_player([&](Polygon* p, std::int32_t width, std::int32_t height) {
			//OpenGL
			//p->ypoints[0] = height - p->ypoints[0];
			//p->ypoints[1] = height - p->ypoints[1];
			//p->ypoints[2] = height - p->ypoints[2];
			//DrawPolyLines(ctx, p->npoints, &p->xpoints[0], &p->ypoints[0]);
			
			
			//OpenGL2
			//DrawPoly(ctx, p->npoints, 1, 0, 0, &p->xpoints[0], &p->ypoints[0]);
			
			
			//Raw
			//draw_polygon(src, p->npoints, &p->ypoints[0], &p->xpoints[0], width, height);
		});
		glPopMatrix();*/
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

#include "NativeHooks.hxx"
#if defined(__linux__)
#include <memory>
#include <thread>
#include "JavaInternal.hxx"
#include "EIOS.hxx"
#include "Graphics.hxx"

#include <gl/gl.h>
#include <gl/glext.h>
#endif


#if defined(__linux__)
typedef void (*JavaNativeBlit_t)(JNIEnv *env, jobject self, jobject srcData, jobject dstData, jobject comp, jobject clip, jint srcx, jint srcy, jint dstx, jint dsty, jint width, jint height);
JavaNativeBlit_t o_JavaNativeBlit;
#endif

#if defined(__linux__)
Bool XShmPutImage(Display *display, Drawable d, GC gc, XImage *image, int src_x, int src_y, int dest_x, int dest_y, unsigned int width, unsigned int height, bool send_event)
{
	extern std::unique_ptr<ControlCenter> control_center;
	if (control_center)
	{
		int bytes_per_pixel = image->bits_per_pixel / 8;
		int stride = width * bytes_per_pixel;
		void *rasBase = reinterpret_cast<std::uint8_t*>(image->data) + (stride * src_y) + (bytes_per_pixel * src_x);
		std::uint8_t* dest = control_center->get_image();
		memcpy(dest, rasBase, (stride / bytes_per_pixel) * height * bytes_per_pixel);

		/*for (std::size_t i = 0; i < image->height; ++i)
		{
			for (std::size_t j = 0; j < image->width; ++j)
			{
				std::uint8_t pixel = image->data[i * image->width + (image->height - j - 1)];
				*dest++ = pixel;
			}
		}*/
	}

    typedef Bool (*XShmPutImage_t*)(Display*, Drawable, GC, XImage*, int, int, int, int, unsigned int, unsigned int, bool)
    static XShmPutImage_t o_XShmPutImage = reinterpret_cast<XShmPutImage_t>(dlsym(RTLD_NEXT, "XShmPutImage"));
    return o_XShmPutImage(display, d, gc, image, src_x, src_y, dest_x, dest_y, width, height, send_event);
}
#endif

#if defined(__linux__)
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

void glXSwapBuffers(Display* dpy, GLXDrawable drawable)
{
	extern std::unique_ptr<ControlCenter> control_center;
	
	if (control_center)
	{
		static GLint ViewPort[4] = {0};
		static GLuint pbo[2] = {0};
		
		glGetIntegerv(GL_VIEWPORT, ViewPort);
		GLint width = ViewPort[2] - ViewPort[0];
		GLint height = ViewPort[3] - ViewPort[1];
		
		std::uint8_t* dest = control_center->get_image();
		GeneratePixelBuffers(ctx, pbo, width, height, 4);
		ReadPixelBuffers(ctx, dest, pbo, width, height, 4);
	}
	
	static decltype(glXSwapBuffers)* o_glXSwapBuffers = reinterpret_cast<decltype(glXSwapBuffers)*>(dlsym(RTLD_NEXT, "glXSwapBuffers"));
	return o_glXSwapBuffers(dpy, drawable);
}
#endif

#if defined(__linux__)
void InitialiseHooks()
{
    #warning "FIX LINUX HOOKS"
}

void StartHook()
{
    InitialiseHooks();
}
#endif

#include "NativeHooks.hxx"
#if defined(__linux__)
#include <memory>
#include <thread>
#include "JavaInternal.hxx"
#include "EIOS.hxx"
#include "Graphics.hxx"
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

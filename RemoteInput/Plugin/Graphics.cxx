#include "Graphics.hxx"
#include <cstdint>
#include <cstring>
#include <memory>

#if defined(_WIN32) || defined(_WIN64)
#if defined(_MSC_VER)
#include <windows.h>
#include <gl/GL.h>

#define GL_PIXEL_PACK_BUFFER              0x88EB
#define GL_STREAM_READ                    0x88E1
#define GL_READ_ONLY                      0x88B8
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_BGRA                           0x80E1
#else
#include <gl/gl.h>
#include <gl/glext.h>
#endif
#elif defined(__APPLE__)
#include <OpenGL/OpenGL.h>
#include <OpenGL/CGLMacro.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

std::uint32_t RGBA(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) noexcept
{
	return (r << 24) + (g << 16) + (b << 8) + (a);
}

void RGBA(std::uint32_t colour, std::uint8_t& r, std::uint8_t& g, std::uint8_t& b, std::uint8_t& a) noexcept
{
	r = (colour >> 24) & 0xFF;
    g = (colour >> 16) & 0xFF;
    b = (colour >> 8) & 0xFF;
    a = (colour & 0xFF);
}

void FlipImageBytes(void* In, void* &Out, int width, int height, uint32_t Bpp) noexcept
{
   unsigned long Chunk = (Bpp > 24 ? width * 4 : width * 3 + width % 4);
   unsigned char* Destination = static_cast<unsigned char*>(Out);
   unsigned char* Source = static_cast<unsigned char*>(In) + Chunk * (height - 1);

   while(Source != In)
   {
      //std::memcpy(Destination, Source, Chunk);
	   for (unsigned long i = 0; i < Chunk; ++i)
	   {
		   *(Destination + i) = *(Source + i);
	   }

      Destination += Chunk;
      Source -= Chunk;
   }
}

void FlipImageVertically(std::int32_t width, std::int32_t height, std::uint8_t* data) noexcept
{
	struct BGRA
	{
		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t a;
	} rgb;

	for (std::int32_t y = 0; y < height / 2; ++y)
	{
		for (std::int32_t x = 0; x < width; ++x)
		{
			std::int32_t top = (x + y * width) * sizeof(BGRA);
			std::int32_t bottom = (x + (height - y - 1) * width) * sizeof(BGRA);

			std::memcpy(&rgb, data + top, sizeof(BGRA));
			std::memcpy(data + top, data + bottom, sizeof(BGRA));
			std::memcpy(data + bottom, &rgb, sizeof(BGRA));
		}
	}
}

void FlipImageVertically2(std::int32_t width, std::int32_t height, std::uint8_t* data) noexcept
{
	const std::size_t stride = width * 4;
	std::unique_ptr<std::uint8_t[]> row = std::make_unique<std::uint8_t[]>(stride);

	for (std::uint8_t* it = data, *jt = &data[(height - 1) * stride]; it < jt; it += stride, jt -= stride)
	{
		std::memcpy(row.get(), it, stride);
		std::memcpy(it, jt, stride);
		std::memcpy(jt, row.get(), stride);
	}
}

/*void draw_circle(std::int32_t x, std::int32_t y, std::int32_t radius, void* buffer, std::int32_t width, std::int32_t height, std::int32_t stride, bool filled) noexcept
{
	typedef struct bgra_t
	{
		std::uint8_t b;
		std::uint8_t g;
		std::uint8_t r;
		std::uint8_t a;
	} bgra;

	auto set_pixel = [&](int x, int y) {
		std::uint8_t* ptr = static_cast<std::uint8_t*>(buffer);
		bgra* pixel = reinterpret_cast<bgra*>(&ptr[(y * width + x) * stride]);
		pixel->r = 0xFF;
		pixel->g = 0xFF;
		pixel->b = 0xFF;
		pixel->a = 0xFF;
	};

	int radiusSq = radius * radius;
	for (int yy = -radius; yy <= radius; ++yy)
	{
		for (int xx = -radius; xx <= radius; ++xx)
		{
			//clamp
			if (x + xx >= 0 && y + yy >= 0 && x + xx <= width && y + yy <= height)
			{
				if (filled)
				{
					if (xx * xx + yy * yy < radiusSq + radius)
					{
						set_pixel(x + xx, y + yy);
					}
				}
				else
				{
					if (xx * xx + yy * yy > radiusSq - radius && xx * xx + yy * yy < radiusSq + radius)
					{
						set_pixel(x + xx, y + yy);
					}
				}
			}
		}
	}
}*/

void draw_circle(std::int32_t x, std::int32_t y, std::int32_t radius, void* buffer, std::int32_t width, std::int32_t height, std::int32_t stride, bool filled, std::int32_t abgr_colour) noexcept
{
	typedef struct bgra_t
	{
		std::uint8_t b;
		std::uint8_t g;
		std::uint8_t r;
		std::uint8_t a;
	} bgra;

	bgra draw_colour = {0};
	RGBA(abgr_colour, draw_colour.r, draw_colour.g, draw_colour.b, draw_colour.a);

	auto set_pixel = [&](int x, int y) {
		std::uint8_t* ptr = static_cast<std::uint8_t*>(buffer);
		bgra* pixel = reinterpret_cast<bgra*>(&ptr[(y * width + x) * stride]);
		*pixel = draw_colour;
	};

	int radiusSq = radius * radius;
	int area = (radiusSq << 2) + (radius << 2) + 1;
	int rr = (radius << 1) + 1;

	for (int i = 0; i < area; ++i)
	{
		int xx = (i % rr) - radius;
		int yy = (i / rr) - radius;

		//clamp
		if (x + xx >= 0 && y + yy >= 0 && x + xx < width && y + yy < height)
		{
			if (filled)
			{
				if (xx * xx + yy * yy < radiusSq + radius)
				{
					set_pixel(x + xx, y + yy);
				}
			}
			else
			{
				if (xx * xx + yy * yy > radiusSq - radius && xx * xx + yy * yy < radiusSq + radius)
				{
					set_pixel(x + xx, y + yy);
				}
			}
		}
	}
}

void draw_image(void* dest_buffer, void* source_buffer, std::int32_t width, std::int32_t height, std::int32_t stride) noexcept
{
	typedef struct bgra_t
	{
		std::uint8_t b;
		std::uint8_t g;
		std::uint8_t r;
		std::uint8_t a;
	} bgra;

	bgra* dest = static_cast<bgra*>(dest_buffer);
	bgra* source = static_cast<bgra*>(source_buffer);

	for (std::int32_t i = 0; i < width * height * stride; i += stride)
	{
        dest->a = (source->b == 0x00 && source->g == 0x00 && source->r == 0x00) ? 0x00 : 0xFF;
	    if (dest->a != 0x00)
        {
            dest->b = source->b;
            dest->g = source->g;
            dest->r = source->r;
        }
		++source;
		++dest;
	}
}

void gl_draw_point(void* ctx, float x, float y, float z, float radius) noexcept
{
	#define GL_TEXTURE_RECTANGLE              0x84F5

	#if defined(__APPLE__)
	CGLContextObj CGL_MACRO_CONTEXT = static_cast<CGLContextObj>(ctx);
	#endif

	//Backup
	GLfloat point_size = 0.0;
	bool GLBlend = glIsEnabled(GL_BLEND);
	bool GLTexture2D = glIsEnabled(GL_TEXTURE_2D);
	bool GLRectangle = glIsEnabled(GL_TEXTURE_RECTANGLE);
    bool PointSmooth = glIsEnabled(GL_POINT_SMOOTH);
	glGetFloatv(GL_POINT_SIZE, &point_size);

	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    glPushMatrix();
    glLoadIdentity();

	//Draw Point
	glRasterPos2f(x, y);
    glPointSize(radius);
    glBegin(GL_POINTS);
        glVertex3f(x, y, z);
    glEnd();
    glFlush();

	//Restore
	glPopMatrix();

	if (!GLBlend)
	{
		glDisable(GL_BLEND);
	}

    if (GLTexture2D)
	{
		glEnable(GL_TEXTURE_2D);
	}

	if (GLRectangle)
	{
		glEnable(GL_TEXTURE_RECTANGLE);
	}

    if (!PointSmooth)
	{
		glDisable(GL_POINT_SMOOTH);
	}

	glPointSize(point_size);
}

void gl_draw_image(void* ctx, void* source_buffer, float x, float y, std::int32_t width, std::int32_t height, std::int32_t stride) noexcept
{
	#define GL_TEXTURE_RECTANGLE              0x84F5

	#if defined(__APPLE__)
	CGLContextObj CGL_MACRO_CONTEXT = static_cast<CGLContextObj>(ctx);
	#endif

	//Backup
	bool GLBlend = glIsEnabled(GL_BLEND);
	bool GLTexture2D = glIsEnabled(GL_TEXTURE_2D);
	bool GLRectangle = glIsEnabled(GL_TEXTURE_RECTANGLE);
    bool PointSmooth = glIsEnabled(GL_POINT_SMOOTH);

	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    glPushMatrix();
    glLoadIdentity();

	//Load Texture
	typedef struct bgra_t
	{
		std::uint8_t b;
		std::uint8_t g;
		std::uint8_t r;
		std::uint8_t a;
	} bgra;

	bgra* source = static_cast<bgra*>(source_buffer);
	for (std::int32_t i = 0; i < width * height * stride; i += stride)
	{
		source->a = (source->b == 0x00 && source->g == 0x00 && source->r == 0x00) ? 0x00 : 0xFF;
		++source;
	}

    static GLuint ID = 0;
	static std::int32_t w = 0;
	static std::int32_t h = 0;

	GLenum target = GL_TEXTURE_RECTANGLE;

	if (ID == 0 || w != width || h != height)
	{
		if (ID != 0)
		{
			glDeleteTextures(1, &ID);
			ID = 0;
		}

		w = width;
		h = height;

		glGenTextures(1, &ID);
		glBindTexture(target, ID);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
		glTexImage2D(target, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, source_buffer);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexParameteri(target, GL_TEXTURE_WRAP_S, target == GL_TEXTURE_2D ? GL_REPEAT : GL_CLAMP_TO_EDGE);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, target == GL_TEXTURE_2D ? GL_REPEAT : GL_CLAMP_TO_EDGE);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else
	{
		glBindTexture(target, ID);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
		glTexSubImage2D(target, 0, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, source_buffer);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glBindTexture(target, 0);
	}


	float x1 = x;
	float y1 = y;
	float x2 = width;
	float y2 = height;
	width = target == GL_TEXTURE_2D ? 1 : width;
    height = target == GL_TEXTURE_2D ? 1 : height;

	//Render Texture
    glEnable(target);
    glBindTexture(target, ID);
    glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);
    glBegin(GL_QUADS);
        glTexCoord2f(0, height);
        glVertex2f(x1, y1);
        glTexCoord2f(0, 0);
        glVertex2f(x1, y2);
        glTexCoord2f(width, 0);
        glVertex2f(x2, y2);
        glTexCoord2f(width, height);
        glVertex2f(x2, y1);
    glEnd();
	glBindTexture(target, 0);
    glDisable(target);


	//Restore
	glPopMatrix();

	if (!GLBlend)
	{
		glDisable(GL_BLEND);
	}

    if (GLTexture2D)
	{
		glEnable(GL_TEXTURE_2D);
	}

	if (GLRectangle)
	{
		glEnable(GL_TEXTURE_RECTANGLE);
	}

    if (!PointSmooth)
	{
		glDisable(GL_POINT_SMOOTH);
	}
}

void dx_draw_point(std::int32_t x, std::int32_t y, std::int32_t radius, void* buffer, std::int32_t width, std::int32_t height, std::int32_t stride, bool filled, std::int32_t argb_colour) noexcept
{
    auto argb_to_abgr = [](std::int32_t argb_colour){
        return ((argb_colour & 0xFF000000) >> 0) |
               ((argb_colour & 0x00FF0000) >>  16) |
               ((argb_colour & 0x0000FF00) <<  0) |
               ((argb_colour & 0x000000FF) << 16);
    };
    return draw_circle(x, y, radius, buffer, width, height, stride, filled, argb_to_abgr(argb_colour));
}

void dx_draw_image(void* dest_buffer, void* source_buffer, std::int32_t width, std::int32_t height, std::int32_t stride) noexcept
{
    typedef struct bgra_t
    {
        std::uint8_t b;
        std::uint8_t g;
        std::uint8_t r;
        std::uint8_t a;
    } bgra;

    typedef struct argb_t
    {
        std::uint8_t a;
        std::uint8_t r;
        std::uint8_t g;
        std::uint8_t b;
    } argb;

    bgra* dest = static_cast<bgra*>(dest_buffer);
    bgra* source = static_cast<bgra*>(source_buffer);

    for (std::int32_t i = 0; i < width * height * stride; i += stride)
    {
        dest->a = (source->b == 0x00 && source->g == 0x00 && source->r == 0x00) ? 0x00 : 0xFF;
        if (dest->a != 0x00)
        {
            dest->b = source->b;
            dest->g = source->g;
            dest->r = source->r;
        }
        ++source;
        ++dest;
    }
}
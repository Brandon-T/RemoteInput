#include "Graphics.hxx"
#include <cstdint>
#include <cstring>
#include <memory>

#if defined(__APPLE__)
#include <OpenGL/OpenGL.h>
#include <OpenGL/CGLMacro.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#else
#include <gl/gl.h>
#include <gl/glext.h>
#endif

/*void draw_circle(std::int32_t x, std::int32_t y, std::int32_t radius, void* buffer, std::int32_t width, std::int32_t height, std::int32_t stride, bool filled)
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

void draw_circle(std::int32_t x, std::int32_t y, std::int32_t radius, void* buffer, std::int32_t width, std::int32_t height, std::int32_t stride, bool filled)
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
	int area = (radiusSq << 2) + (radius << 2) + 1;
	int rr = (radius << 1) + 1;
	
	for (int i = 0; i < area; ++i)
	{
		int xx = (i % rr) - radius;
		int yy = (i / rr) - radius;
		
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

void draw_image(void* dest_buffer, void* source_buffer, std::int32_t width, std::int32_t height, std::int32_t stride)
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
		dest->b = source->b;
		dest->g = source->g;
		dest->r = source->r;
		dest->a = (source->b == 0x00 && source->g == 0x00 && source->r == 0x00) ? 0x00 : 0xFF;
		++source;
	}
}

void gl_draw_image(void* ctx, void* source_buffer, std::int32_t width, std::int32_t height, std::int32_t stride)
{
	#if defined(__APPLE__)
	CGLContextObj CGL_MACRO_CONTEXT = static_cast<CGLContextObj>(ctx);
	#endif
	
	/*auto flip_in_place = [](std::uint8_t* data, std::int32_t width, std::int32_t height) {
		struct bgra_t {
			std::uint8_t b;
			std::uint8_t g;
			std::uint8_t r;
			std::uint8_t a;
		} bgra;
		
		for (int y = 0; y < height / 2; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				int top = (x + y * width) * sizeof(bgra_t);
				int bottom = (x + (height - y - 1) * width) * sizeof(bgra_t);

				std::memcpy(&rgb, data + top, sizeof(bgra_t));
				std::memcpy(data + top, data + bottom, sizeof(bgra_t));
				std::memcpy(data + bottom, &rgb, sizeof(bgra_t));
			}
		}
	};*/
	
	auto flip_to_buffer = [](std::uint8_t* dest, std::uint8_t* source, std::int32_t width, std::int32_t height, std::int32_t stride) {
		std::uint32_t chunk = width * stride;
		std::uint8_t* ptr = source + chunk * (height - 1);

	   while(ptr != source)
	   {
		  std::memcpy(dest, ptr, chunk);
		  dest += chunk;
		  ptr -= chunk;
	   }
	};
	
	//Backup
	bool GLTexture2D = glIsEnabled(GL_TEXTURE_2D);
    bool PointSmooth = glIsEnabled(GL_POINT_SMOOTH);

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    glPushMatrix();
    glLoadIdentity();
	
	//Load Texture
	std::unique_ptr<std::uint8_t[]> px { new std::uint8_t[width * height * stride] };
	flip_to_buffer(px.get(), static_cast<std::uint8_t*>(source_buffer), width, height, stride);
	
	for (std::int32_t i = 0; i < width * height * stride; i += stride)
	{
		std::uint8_t* source = px.get() + i;
		std::uint8_t* ptr = px.get() + i + 3;
		*ptr = (source[0] == 0x00 && source[1] == 0x00 && source[2] == 0x00) ? 0x00 : 0xFF;
	}

    GLuint ID = 0;
	GLenum target = GL_TEXTURE_2D;
	
    glGenTextures(1, &ID);
    glBindTexture(target, ID);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
    glTexImage2D(target, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, px.get());
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, target == GL_TEXTURE_2D ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, target == GL_TEXTURE_2D ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	width = target == GL_TEXTURE_2D ? 1 : width;
    height = target == GL_TEXTURE_2D ? 1 : height;

	//Render Texture
	float x1 = 0.0;
	float y1 = 0.0;
	float x2 = 1.0;
	float y2 = 1.0;
	
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
    glDisable(target);
	glDeleteTextures(1, &ID);
	
	//Restore
	glPopMatrix();
    if (GLTexture2D)
	{
		glEnable(GL_TEXTURE_2D);
	}
	
    if (!PointSmooth)
	{
		glDisable(GL_POINT_SMOOTH);
	}
}

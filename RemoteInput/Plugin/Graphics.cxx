#include "Graphics.hxx"

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

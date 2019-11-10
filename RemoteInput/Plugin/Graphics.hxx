#ifndef Graphics_HXX_INCLUDED
#define Graphics_HXX_INCLUDED

#include <cstdint>

//[[gnu::optimize(0)]]
//[[clang::optimize(0)]] //optnone

#pragma GCC push_options
#pragma GCC optimize ("-O3")
void draw_circle(std::int32_t x, std::int32_t y, std::int32_t radius, void* buffer, std::int32_t width, std::int32_t height, std::int32_t stride, bool filled);

void draw_image(void* dest_buffer, void* source_buffer, std::int32_t width, std::int32_t height, std::int32_t stride);
#pragma GCC pop_options

#endif /* Graphics_HXX_INCLUDED */

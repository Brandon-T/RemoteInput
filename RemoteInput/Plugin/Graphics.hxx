#ifndef Graphics_HXX_INCLUDED
#define Graphics_HXX_INCLUDED

#include <cstdint>

//[[gnu::optimize(0)]]
//[[clang::optimize(0)]] //optnone

#pragma GCC push_options
#pragma GCC optimize ("-O3")
void FlipImageBytes(void* In, void* &Out, int width, int height, uint32_t Bpp) noexcept;
void FlipImageVertically(std::int32_t width, std::int32_t height, std::uint8_t* data) noexcept;
void FlipImageVertically2(std::int32_t width, std::int32_t height, std::uint8_t* data) noexcept;

//Native
void draw_circle(std::int32_t x, std::int32_t y, std::int32_t radius, void* buffer, std::int32_t width, std::int32_t height, std::int32_t stride, bool filled, std::int32_t abgr_colour) noexcept;
void draw_image(void* dest_buffer, void* source_buffer, std::int32_t width, std::int32_t height, std::int32_t stride) noexcept;

//OpenGL
void gl_draw_point(void* ctx, float x, float y, float z, float radius) noexcept;
void gl_draw_image(void* ctx, void* source_buffer, float x, float y, std::int32_t width, std::int32_t height, std::int32_t stride) noexcept;

//DirectX
void dx_draw_point(std::int32_t x, std::int32_t y, std::int32_t radius, void* buffer, std::int32_t width, std::int32_t height, std::int32_t stride, bool filled, std::int32_t argb_colour) noexcept;
void dx_draw_image(void* dest_buffer, void* source_buffer, std::int32_t width, std::int32_t height, std::int32_t stride) noexcept;
#pragma GCC pop_options

#endif /* Graphics_HXX_INCLUDED */

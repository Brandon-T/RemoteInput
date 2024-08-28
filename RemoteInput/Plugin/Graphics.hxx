#ifndef Graphics_HXX_INCLUDED
#define Graphics_HXX_INCLUDED

#include "EIOSTypes.hxx"
#include <cstdint>
#include <cstring>
#include <type_traits>

//[[gnu::optimize(0)]]
//[[clang::optimize(0)]] //optnone

#if defined(_MSC_VER)
#pragma optimize("s", on)
#else
#pragma GCC push_options
#pragma GCC optimize ("-O3")
#endif

typedef struct bgr_bgra_t
{
    std::uint8_t b;
    std::uint8_t g;
    std::uint8_t r;
    std::uint8_t a;
} bgr_bgra;

typedef struct abgr_t
{
    std::uint8_t a;
    std::uint8_t b;
    std::uint8_t g;
    std::uint8_t r;
} abgr;

typedef struct argb_t
{
    std::uint8_t a;
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
} argb;

typedef struct rgba_t
{
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a;
} rgba;

typedef struct bgra_t
{
    std::uint8_t b;
    std::uint8_t g;
    std::uint8_t r;
    std::uint8_t a;
} bgra;

template<typename S, typename D>
void convert_pixels(S source, D dest, std::int32_t width, std::int32_t height, std::int32_t stride)
{
    if constexpr(std::is_same<S, D>::value)
    {
        std::memcpy(dest, source, width * height * stride);
    }
    else
    {
        for (std::int32_t i = 0; i < width * height * stride; i += stride)
        {
            dest->r = source->r;
            dest->g = source->g;
            dest->b = source->b;
            dest->a = 0xFF; // source->a;
            ++source;
            ++dest;
        }
    }
};

template<typename S, typename D>
void alpha_blend_pixels(S source, D dest, std::int32_t width, std::int32_t height, std::int32_t stride)
{
    auto blend_alpha = [](std::uint8_t back, std::uint8_t front, std::uint8_t alpha) -> std::uint8_t {
        return ((front * alpha) + (back * (0xFF - alpha))) / 0xFF;
    };

    for (std::int32_t i = 0; i < width * height * stride; i += stride)
    {
        if constexpr(std::is_same<S, D>::value && std::is_same<S, bgr_bgra_t*>::value)
        {
            dest->a = *reinterpret_cast<std::uint32_t*>(source) == 0x00 ? 0x00 : 0xFF;
            if (dest->a != 0x00)
            {
                dest->r = source->r;
                dest->g = source->g;
                dest->b = source->b;
            }
        }
        else
        {
            // Pre-Multiplied Alpha
            dest->r = blend_alpha(dest->r, source->r, source->a);
            dest->g = blend_alpha(dest->g, source->g, source->a);
            dest->b = blend_alpha(dest->b, source->b, source->a);
            dest->a = 0xFF;
        }

        ++source;
        ++dest;
    }
}

void FlipImageBytes(void* In, void* Out, std::int32_t width, std::int32_t height, std::uint32_t Bpp) noexcept;
void FlipImageVertically(std::int32_t width, std::int32_t height, std::uint8_t* data) noexcept;
void FlipImageVertically2(std::int32_t width, std::int32_t height, std::uint8_t* data) noexcept;

//Native
void draw_circle(std::int32_t x, std::int32_t y, std::int32_t radius, void* buffer, std::int32_t width, std::int32_t height, std::int32_t stride, bool filled, std::int32_t abgr_colour) noexcept;
void copy_image(void* dest_buffer, void* source_buffer, std::int32_t width, std::int32_t height, std::int32_t stride, ImageFormat format) noexcept;
void draw_image(void* dest_buffer, void* source_buffer, std::int32_t width, std::int32_t height, std::int32_t stride, ImageFormat format) noexcept;

//OpenGL
void gl_draw_point(void* ctx, float x, float y, float z, float radius) noexcept;
void gl_draw_image(void* ctx, void* source_buffer, float x, float y, std::int32_t width, std::int32_t height, std::int32_t stride, ImageFormat format) noexcept;

//DirectX
void dx_draw_point(std::int32_t x, std::int32_t y, std::int32_t radius, void* buffer, std::int32_t width, std::int32_t height, std::int32_t stride, bool filled, std::int32_t argb_colour) noexcept;
void dx_draw_image(void* dest_buffer, void* source_buffer, std::int32_t width, std::int32_t height, std::int32_t stride, ImageFormat format) noexcept;

#if defined(_MSC_VER)
#pragma optimize("", on)
#else
#pragma GCC pop_options
#endif

#endif /* Graphics_HXX_INCLUDED */

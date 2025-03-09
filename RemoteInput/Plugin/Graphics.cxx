#include "Graphics.hxx"
#include "Thirdparty/Hook.hxx"
#include <cstdint>
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

// ARGB To Format

std::uint32_t argb_to_abgr(std::uint32_t argb_colour)
{
    std::uint8_t a = (argb_colour >> 24) & 0xFF;
    std::uint8_t r = (argb_colour >> 16) & 0xFF;
    std::uint8_t g = (argb_colour >> 8)  & 0xFF;
    std::uint8_t b = (argb_colour >> 0)  & 0xFF;
    return (a << 24) | (b << 16) | (g << 8) | r;
}

std::uint32_t argb_to_bgra(std::uint32_t argb_colour)
{
    std::uint8_t a = (argb_colour >> 24) & 0xFF;
    std::uint8_t r = (argb_colour >> 16) & 0xFF;
    std::uint8_t g = (argb_colour >> 8)  & 0xFF;
    std::uint8_t b = (argb_colour >> 0)  & 0xFF;
    return (b << 24) | (g << 16) | (r << 8) | a;
}

std::uint32_t argb_to_rgba(std::uint32_t argb_colour)
{
    std::uint8_t a = (argb_colour >> 24) & 0xFF;
    std::uint8_t r = (argb_colour >> 16) & 0xFF;
    std::uint8_t g = (argb_colour >> 8)  & 0xFF;
    std::uint8_t b = (argb_colour >> 0)  & 0xFF;
    return (r << 24) | (g << 16) | (b << 8) | a;
}

// ABGR to Format

std::uint32_t abgr_to_argb(std::uint32_t abgr_colour)
{
    std::uint8_t a = (abgr_colour >> 24) & 0xFF;
    std::uint8_t b = (abgr_colour >> 16) & 0xFF;
    std::uint8_t g = (abgr_colour >> 8)  & 0xFF;
    std::uint8_t r = (abgr_colour >> 0)  & 0xFF;
    return (a << 24) | (r << 16) | (g << 8) | b;
}

std::uint32_t abgr_to_bgra(std::uint32_t abgr_colour)
{
    std::uint8_t a = (abgr_colour >> 24) & 0xFF;
    std::uint8_t b = (abgr_colour >> 16) & 0xFF;
    std::uint8_t g = (abgr_colour >> 8)  & 0xFF;
    std::uint8_t r = (abgr_colour >> 0)  & 0xFF;
    return (a << 24) | (b << 16) | (g << 8) | r;
}

std::uint32_t abgr_to_rgba(std::uint32_t abgr_colour)
{
    std::uint8_t a = (abgr_colour >> 24) & 0xFF;
    std::uint8_t b = (abgr_colour >> 16) & 0xFF;
    std::uint8_t g = (abgr_colour >> 8)  & 0xFF;
    std::uint8_t r = (abgr_colour >> 0)  & 0xFF;
    return (r << 24) | (g << 16) | (b << 8) | a;
}

// RGBA to Format

std::uint32_t rgba_to_argb(std::uint32_t rgba_colour)
{
    std::uint8_t r = (rgba_colour >> 24) & 0xFF;
    std::uint8_t g = (rgba_colour >> 16) & 0xFF;
    std::uint8_t b = (rgba_colour >> 8)  & 0xFF;
    std::uint8_t a = (rgba_colour >> 0)  & 0xFF;
    return (a << 24) | (r << 16) | (g << 8) | b;
}

std::uint32_t rgba_to_abgr(std::uint32_t rgba_colour)
{
    std::uint8_t r = (rgba_colour >> 24) & 0xFF;
    std::uint8_t g = (rgba_colour >> 16) & 0xFF;
    std::uint8_t b = (rgba_colour >> 8)  & 0xFF;
    std::uint8_t a = (rgba_colour >> 0)  & 0xFF;
    return (a << 24) | (b << 16) | (g << 8) | r;
}

std::uint32_t rgba_to_bgra(std::uint32_t rgba_colour)
{
    std::uint8_t r = (rgba_colour >> 24) & 0xFF;
    std::uint8_t g = (rgba_colour >> 16) & 0xFF;
    std::uint8_t b = (rgba_colour >> 8)  & 0xFF;
    std::uint8_t a = (rgba_colour >> 0)  & 0xFF;
    return (b << 24) | (g << 16) | (r << 8) | a;
}

// BGRA to Format

std::uint32_t bgra_to_abgr(std::uint32_t bgra_colour)
{
    std::uint8_t b = (bgra_colour >> 24) & 0xFF;
    std::uint8_t g = (bgra_colour >> 16) & 0xFF;
    std::uint8_t r = (bgra_colour >> 8)  & 0xFF;
    std::uint8_t a = (bgra_colour >> 0)  & 0xFF;
    return (a << 24) | (b << 16) | (g << 8) | r;
}

std::uint32_t bgra_to_argb(std::uint32_t bgra_colour)
{
    std::uint8_t b = (bgra_colour >> 24) & 0xFF;
    std::uint8_t g = (bgra_colour >> 16) & 0xFF;
    std::uint8_t r = (bgra_colour >> 8)  & 0xFF;
    std::uint8_t a = (bgra_colour >> 0)  & 0xFF;
    return (a << 24) | (r << 16) | (g << 8) | b;
}

std::uint32_t bgra_to_rgba(std::uint32_t bgra_colour)
{
    std::uint8_t b = (bgra_colour >> 24) & 0xFF;
    std::uint8_t g = (bgra_colour >> 16) & 0xFF;
    std::uint8_t r = (bgra_colour >> 8)  & 0xFF;
    std::uint8_t a = (bgra_colour >> 0)  & 0xFF;
    return (r << 24) | (g << 16) | (b << 8) | a;
}

// Other

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

void FlipImageBytes(void* In, void* Out, std::int32_t width, std::int32_t height, std::uint32_t Bpp) noexcept
{
    std::size_t Chunk = (Bpp > 24 ? width * 4 : width * 3 + width % 4);
   std::uint8_t* Destination = static_cast<std::uint8_t*>(Out);
    std::uint8_t* Source = static_cast<std::uint8_t*>(In) + Chunk * (height - 1);

   while(Source != In)
   {
      //std::memcpy(Destination, Source, Chunk);
       for (std::size_t i = 0; i < Chunk; ++i)
       {
           *(Destination + i) = *(Source + i);
       }

      Destination += Chunk;
      Source -= Chunk;
   }
}

void FlipImageVertically(std::int32_t width, std::int32_t height, std::uint8_t* data) noexcept
{
    bgra_t pixel = {0};
    for (std::int32_t y = 0; y < height / 2; ++y)
    {
        for (std::int32_t x = 0; x < width; ++x)
        {
            std::uint32_t top = (x + y * width) * sizeof(bgra_t);
            std::uint32_t bottom = (x + (height - y - 1) * width) * sizeof(bgra_t);

            std::memcpy(&pixel, data + top, sizeof(bgra_t));
            std::memcpy(data + top, data + bottom, sizeof(bgra_t));
            std::memcpy(data + bottom, &pixel, sizeof(bgra_t));
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

void draw_circle(std::int32_t x, std::int32_t y, std::int32_t radius, void* buffer, std::int32_t width, std::int32_t height, std::int32_t stride, bool filled, std::int32_t abgr_colour) noexcept
{
    bgra_t draw_colour = {0};
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

void copy_image(void* dest_buffer, void* source_buffer, std::int32_t width, std::int32_t height, std::int32_t stride, ImageFormat format) noexcept
{
    switch (format)
    {
        case ImageFormat::BGR_BGRA:
            convert_pixels(static_cast<bgr_bgra_t*>(source_buffer), static_cast<bgr_bgra_t*>(dest_buffer), width, height, stride);
            break;

        case ImageFormat::BGRA:
            convert_pixels(static_cast<bgra_t*>(source_buffer), static_cast<bgra_t*>(dest_buffer), width, height, stride);
            break;

        case ImageFormat::RGBA:
            convert_pixels(static_cast<bgra_t*>(source_buffer), static_cast<rgba_t*>(dest_buffer), width, height, stride);
            break;

        case ImageFormat::ARGB:
            convert_pixels(static_cast<bgra_t*>(source_buffer), static_cast<argb_t*>(dest_buffer), width, height, stride);
            break;

        case ImageFormat::ABGR:
            convert_pixels(static_cast<bgra_t*>(source_buffer), static_cast<abgr_t*>(dest_buffer), width, height, stride);
            break;
    }
}

void draw_image(void* dest_buffer, void* source_buffer, std::int32_t width, std::int32_t height, std::int32_t stride, ImageFormat format) noexcept
{
    switch (format)
    {
        case ImageFormat::BGR_BGRA:
            alpha_blend_pixels(static_cast<bgr_bgra_t*>(source_buffer), static_cast<bgr_bgra_t*>(dest_buffer), width, height, stride);
            break;

        case ImageFormat::BGRA:
            alpha_blend_pixels(static_cast<bgra_t*>(source_buffer), static_cast<bgra_t*>(dest_buffer), width, height, stride);
            break;

        case ImageFormat::RGBA:
            alpha_blend_pixels(static_cast<rgba_t*>(source_buffer), static_cast<bgra_t*>(dest_buffer), width, height, stride);
            break;

        case ImageFormat::ARGB:
            alpha_blend_pixels(static_cast<argb_t*>(source_buffer), static_cast<bgra_t*>(dest_buffer), width, height, stride);
            break;

        case ImageFormat::ABGR:
            alpha_blend_pixels(static_cast<abgr_t*>(source_buffer), static_cast<bgra_t*>(dest_buffer), width, height, stride);
            break;
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

void gl_draw_image(void* ctx, void* source_buffer, float x, float y, std::int32_t width, std::int32_t height, std::int32_t stride, ImageFormat format) noexcept
{
    #define GL_TEXTURE_RECTANGLE              0x84F5

    #if defined(__APPLE__)
    CGLContextObj CGL_MACRO_CONTEXT = static_cast<CGLContextObj>(ctx);
    #endif

    GLenum gl_format = [](ImageFormat format) -> GLenum {
        switch(format)
        {
            case ImageFormat::BGR_BGRA: return GL_BGRA;
            case ImageFormat::BGRA: return GL_BGRA;
            case ImageFormat::RGBA: return GL_RGBA;
            case ImageFormat::ARGB: return 0;  // Not Supported
            case ImageFormat::ABGR: return 0;  // Not Supported
            default: return GL_BGRA;
        }
    }(format);

    auto convert = []<typename S>(S source, std::int32_t width, std::int32_t height, std::int32_t stride, ImageFormat format) {
        if constexpr(std::is_same<S, bgr_bgra_t>::value)
        {
            for (std::int32_t i = 0; i < width * height * stride; i += stride)
            {
                source->a = *reinterpret_cast<std::uint32_t*>(source) == 0x00 ? 0x00 : 0xFF;
                ++source;
            }
        }
    };

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
    switch (format)
    {
        case ImageFormat::BGR_BGRA:
            convert(static_cast<bgr_bgra_t*>(source_buffer), width, height, stride, format);
            break;

        case ImageFormat::BGRA:
            convert(static_cast<bgra_t*>(source_buffer), width, height, stride, format);
            break;

        case ImageFormat::RGBA:
            convert(static_cast<rgba_t*>(source_buffer), width, height, stride, format);
            break;

        case ImageFormat::ARGB:
            convert(static_cast<argb_t*>(source_buffer), width, height, stride, format);
            break;

        case ImageFormat::ABGR:
            convert(static_cast<abgr_t*>(source_buffer), width, height, stride, format);
            break;

        default:
            convert(static_cast<bgra_t*>(source_buffer), width, height, stride, format);
            break;
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
        glTexImage2D(target, 0, GL_RGBA, width, height, 0, gl_format, GL_UNSIGNED_BYTE, source_buffer);
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
        glTexSubImage2D(target, 0, 0, 0, width, height, gl_format, GL_UNSIGNED_BYTE, source_buffer);
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

#if defined(_WIN32) || defined(_WIN64)
#define VERTEX_FVF_TEX D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1

struct D3DVertex
{
    float X, Y, Z, RHW;
    unsigned int Colour;
    float U, V;
};

HMODULE dx_get_d3dx9_module() noexcept
{
    // [24, 43] d3dx9 versions
    char d3dx9_version[32] = {0};
    for (int i = 43; i > 23; --i)
    {
        sprintf(d3dx9_version, "d3dx9_%d.dll", i);
    }

    HMODULE module = GetModuleHandle(d3dx9_version);
    if (!module)
    {
        module = LoadLibrary(d3dx9_version);
    }

    return module;
}

IDirect3DPixelShader9* dx_texture_render_shader(IDirect3DDevice9* device, ID3DXConstantTable** table)
{
    static const char* shader_code = R"(
        sampler2D tex;
        int format_id;

        struct PS_INPUT {
            float2 texCoord : TEXCOORD0;
        };

        float4 PS_Main(PS_INPUT input) : COLOR {
            float4 color = tex2D(tex, input.texCoord);
            float4 output = color;

            if (format_id == 0) {
                if (color.r == 0 && color.g == 0 && color.b == 0) {
                    output = float4(color.r, color.g, color.b, 0.0);
                }
                else {
                    output = float4(color.r, color.g, color.b, 1.0);
                }
            }
            else if (format_id == 1) {
                output = color;
            }
            else if (format_id == 2) {
                output = float4(color.b, color.g, color.r, color.a);
            }
            else if (format_id == 3) {
                output = float4(color.a, color.r, color.g, color.b);
            }
            else if (format_id == 4) {
                output = float4(color.a, color.b, color.g, color.r);
            }

            return output;
        }
    )";

    static ID3DXConstantTable* constant_table = nullptr;
    static IDirect3DPixelShader9* shader = dx_compile_shader(device, constant_table, shader_code);

    if (table)
    {
        *table = constant_table;
    }

    return shader;
}

IDirect3DPixelShader9* dx_read_backbuffer_shader(IDirect3DDevice9* device, ID3DXConstantTable** table)
{
    static const char* shader_code = R"(
        sampler2D screen_texture;
        int format_id;

        struct PS_INPUT {
            float2 texCoord : TEXCOORD0;
        };

        float4 PS_Main(PS_INPUT input) : COLOR {
            float4 color = tex2D(screen_texture, input.texCoord);
            float4 output = color;

            if (format_id == 0) {
                if (color.r == 0 && color.g == 0 && color.b == 0) {
                    output = float4(color.r, color.g, color.b, 0.0);
                }
                else {
                    output = float4(color.r, color.g, color.b, 1.0);
                }
            }
            else if (format_id == 1) {
                output = color;
            }
            else if (format_id == 2) {
                output = float4(color.r, color.g, color.b, color.a);
            }
            else if (format_id == 3) {
                output = float4(color.a, color.r, color.g, color.b);
            }
            else if (format_id == 4) {
                output = float4(color.a, color.b, color.g, color.r);
            }

            return output;
        }
    )";

    static ID3DXConstantTable* constant_table = nullptr;
    static IDirect3DPixelShader9* shader = dx_compile_shader(device, constant_table, shader_code);

    if (table)
    {
        *table = constant_table;
    }

    return shader;
}

IDirect3DPixelShader9* dx_compile_shader(IDirect3DDevice9* device, ID3DXConstantTable* &constant_table, const std::string &shader_string) noexcept
{
    HMODULE module = dx_get_d3dx9_module();
    if (!module)
    {
        return nullptr;
    }

    auto* pD3DXCompileShader = reinterpret_cast<decltype(D3DXCompileShader)*>(GetProcAddress(module, "D3DXCompileShader"));
    if (!pD3DXCompileShader)
    {
        return nullptr;
    }

    ID3DXBuffer* shader_buffer = nullptr;
    ID3DXBuffer* error_buffer = nullptr;

    HRESULT hr = pD3DXCompileShader(
            shader_string.c_str(),
            shader_string.length(),
            nullptr,
            nullptr,
            "PS_Main",
            "ps_2_0",
            D3DXSHADER_DEBUG,
            &shader_buffer,
            &error_buffer,
            &constant_table
    );

    if (FAILED(hr))
    {
        if (error_buffer)
        {
            error_buffer->Release();
        }

        return nullptr;
    }

    if (shader_buffer)
    {
        IDirect3DPixelShader9* pixel_shader = nullptr;
        hr = device->CreatePixelShader(reinterpret_cast<DWORD*>(shader_buffer->GetBufferPointer()), &pixel_shader);
        shader_buffer->Release();

        if (FAILED(hr))
        {
            if (error_buffer)
            {
                error_buffer->Release();
            }
            return nullptr;
        }

        return pixel_shader;
    }

    return nullptr;
}

void dx_apply_shader(IDirect3DDevice9* device, IDirect3DPixelShader9* shader, ID3DXConstantTable* constant_table, ImageFormat format) noexcept
{
    std::int32_t format_id = [](ImageFormat format) -> std::int32_t {
        switch (format)
        {
            case ImageFormat::BGR_BGRA: return 0;
            case ImageFormat::BGRA:     return 1;
            case ImageFormat::RGBA:     return 2;
            case ImageFormat::ARGB:     return 3;
            case ImageFormat::ABGR:     return 4;
        }
        return 0; // Default to BGR_BGRA
    }(format);

    if (constant_table)
    {
        D3DXHANDLE format_handle = constant_table->GetConstantByName(nullptr, "format_id");
        if (!format_handle)
        {
            return;
        }

        constant_table->SetInt(device, format_handle, format_id);
    }

    device->SetPixelShader(shader);
}

void dx_load_texture(IDirect3DDevice9* device, IDirect3DTexture9* &texture, ImageFormat image_format, std::uint8_t* buffer, std::int32_t width, std::int32_t height) noexcept
{
    static std::int32_t Width = 0;
    static std::int32_t Height = 0;

    if (Width != width || Height != height)
    {
        if (texture)
        {
            texture->Release();
            texture = nullptr;
        }

        Width = width;
        Height = height;
    }

    if (!texture)
    {
        HRESULT hr = device->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture, nullptr);
        if (FAILED(hr))
        {
            if (texture)
            {
                texture->Release();
                texture = nullptr;
            }
            return;
        }
    }

    D3DLOCKED_RECT rect;
    if (FAILED(texture->LockRect(0, &rect, nullptr, D3DLOCK_DISCARD)))
    {
        if (texture)
        {
            texture->Release();
            texture = nullptr;
        }
        return;
    }

    IDirect3DPixelShader9* shader = dx_texture_render_shader(device, nullptr);
    std::uint8_t* dest = static_cast<std::uint8_t*>(rect.pBits);
    int pitch = rect.Pitch;

    if (shader)
    {
        for (int i = 0; i < height; ++i)
        {
            std::memcpy(dest + i * pitch, buffer + i * width * 4, width * 4);
        }
    }
    else
    {
        for (int i = 0; i < height; ++i)
        {
            copy_image(dest + i * pitch, buffer + i * width * 4, width, 1, 4, image_format);
        }
    }

    texture->UnlockRect(0);
}

void dx_draw_texture(IDirect3DDevice9* device, IDirect3DTexture9* texture, ImageFormat image_format, float X1, float Y1, float X2, float Y2) noexcept
{
    static std::int32_t Width = static_cast<std::int32_t>(X2 - X1);
    static std::int32_t Height = static_cast<std::int32_t>(Y2 - Y1);
    static IDirect3DVertexBuffer9* vertex_buffer = nullptr;

    // Compile the shader just once
    static ID3DXConstantTable* constant_table = nullptr;
    static IDirect3DPixelShader9* shader = dx_texture_render_shader(device, &constant_table);

    float UOffset = 0.5f / (X2 - X1);
    float VOffset = 0.5f / (Y2 - Y1);

    D3DVertex vertices[] = {
        {X1, Y1, 1.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 255), 0.0f + UOffset, 0.0f + VOffset},
        {X2, Y1, 1.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 255), 1.0f + UOffset, 0.0f + VOffset},
        {X1, Y2, 1.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 255), 0.0f + UOffset, 1.0f + VOffset},
        {X2, Y2, 1.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 255), 1.0f + UOffset, 1.0f + VOffset}
    };

    if (!vertex_buffer)
    {
        HRESULT res = device->CreateVertexBuffer(4 * sizeof(D3DVertex), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, D3DPOOL_DEFAULT, &vertex_buffer, nullptr);
        if (FAILED(res))
        {
            if (vertex_buffer)
            {
                vertex_buffer->Release();
                vertex_buffer = nullptr;
            }
        }
        else
        {
            void *pVertices;
            if (SUCCEEDED(vertex_buffer->Lock(0, sizeof(vertices), (void **) &pVertices, D3DLOCK_DISCARD)))
            {
                std::memcpy(pVertices, vertices, sizeof(vertices));
                vertex_buffer->Unlock();
            }
        }
    }
    else
    {
        if (Width != static_cast<std::int32_t>(X2 - X1) || Height != static_cast<std::int32_t>(Y2 - Y1))
        {
            Width = static_cast<std::int32_t>(X2 - X1);
            Height = static_cast<std::int32_t>(Y2 - Y1);

            void *pVertices;
            if (SUCCEEDED(vertex_buffer->Lock(0, sizeof(vertices), (void **) &pVertices, D3DLOCK_DISCARD)))
            {
                std::memcpy(pVertices, vertices, sizeof(vertices));
                vertex_buffer->Unlock();
            }
        }
    }

    if (shader)
    {
        dx_apply_shader(device, shader, constant_table, image_format);
    }

    device->SetFVF(VERTEX_FVF_TEX);
    device->SetTexture(0, texture);

    if (vertex_buffer)
    {
        device->SetStreamSource(0, vertex_buffer, 0, sizeof(D3DVertex));
        device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
        device->SetStreamSource(0, nullptr, 0, 0);
    }
    else
    {
        device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(D3DVertex));
    }

    device->SetTexture(0, nullptr);

    if (shader)
    {
        device->SetPixelShader(nullptr);
    }
}

void dx_read_pixels(IDirect3DDevice9* device, void* buffer, std::int32_t &width, std::int32_t &height, bool& minimized, ImageFormat image_format) noexcept
{
    // Compile the shader just once
    static ID3DXConstantTable* constant_table = nullptr;
    static IDirect3DPixelShader9* shader = nullptr; //dx_read_backbuffer_shader(device, &constant_table);

    IDirect3DSurface9* render_target = nullptr;
    IDirect3DSurface9* dest_target = nullptr;

    #define SAFE_RELEASE(ptr) if (ptr) ptr->Release()

    HRESULT result = device->GetRenderTarget(0, &render_target);
    if (FAILED(result))
    {
        SAFE_RELEASE(render_target);
        return;
    }

    D3DSURFACE_DESC descriptor = {};
    render_target->GetDesc(&descriptor);
    width = static_cast<std::int32_t>(descriptor.Width);
    height = static_cast<std::int32_t>(descriptor.Height);

    D3DFORMAT format = descriptor.Format;

    HDC DC = nullptr;
    render_target->GetDC(&DC);
    minimized = IsIconic(WindowFromDC(DC));
    render_target->ReleaseDC(DC);

    result = device->CreateOffscreenPlainSurface(width, height, format, D3DPOOL_SYSTEMMEM, &dest_target, nullptr);
    if (FAILED(result))
    {
        SAFE_RELEASE(dest_target);
        SAFE_RELEASE(render_target);
        return;
    }

    if (shader)
    {
        // Texture to store converted pixels
        IDirect3DTexture9* backbuffer_texture = nullptr;
        result = device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, format, D3DPOOL_DEFAULT, &backbuffer_texture, nullptr);
        if (FAILED(result))
        {
            SAFE_RELEASE(backbuffer_texture);
            SAFE_RELEASE(dest_target);
            SAFE_RELEASE(render_target);
            return;
        }

        // Render the Screen to the Texture
        IDirect3DSurface9* backbuffer_texture_surface = nullptr;
        backbuffer_texture->GetSurfaceLevel(0, &backbuffer_texture_surface);

        result = device->StretchRect(render_target, nullptr, backbuffer_texture_surface, nullptr, D3DTEXF_NONE);
        if (FAILED(result))
        {
            SAFE_RELEASE(backbuffer_texture_surface);
            SAFE_RELEASE(backbuffer_texture);
            SAFE_RELEASE(dest_target);
            SAFE_RELEASE(render_target);
            return;
        }

        // Create a texture to render the back-buffer with the shader applied
        IDirect3DTexture9* final_texture = nullptr;
        result = device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, format, D3DPOOL_DEFAULT, &final_texture, nullptr);
        if (FAILED(result))
        {
            SAFE_RELEASE(final_texture);
            SAFE_RELEASE(backbuffer_texture_surface);
            SAFE_RELEASE(backbuffer_texture);
            SAFE_RELEASE(dest_target);
            SAFE_RELEASE(render_target);
            return;
        }

        IDirect3DSurface9* final_texture_surface = nullptr;
        final_texture->GetSurfaceLevel(0, &final_texture_surface);

        // Set the render target to the final texture
        device->SetRenderTarget(0, final_texture_surface);

        // Apply the pixel format shader
        dx_apply_shader(device, shader, constant_table, image_format);

        // Draw the backbuffer_texture to the final_texture with the shader applied
        float UOffset = 0.5f / static_cast<float>(width);
        float VOffset = 0.5f / static_cast<float>(height);

        D3DVertex vertices[] = {
            {0.0f, 0.0f, 1.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 255), 0.0f + UOffset, 0.0f + VOffset},
            {static_cast<float>(width), 0.0f, 1.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 255), 1.0f + UOffset, 0.0f + VOffset},
            {0.0f, static_cast<float>(height), 1.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 255), 0.0f + UOffset, 1.0f + VOffset},
            {static_cast<float>(width), static_cast<float>(height), 1.0f, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 255), 1.0f + UOffset, 1.0f + VOffset}
        };

        device->SetFVF(VERTEX_FVF_TEX);
        device->SetTexture(0, backbuffer_texture);

        // Render the backbuffer_texture to the final_texture
        device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(D3DVertex));   // NO IDEA WHY. THIS LINE STOPS RENDERING ENTIRELY

        // Reset render target and shader
        device->SetRenderTarget(0, render_target);
        device->SetPixelShader(nullptr);

        // Copy the final texture to destination
        result = device->GetRenderTargetData(final_texture_surface, dest_target);
        if (FAILED(result))
        {
            SAFE_RELEASE(final_texture_surface);
            SAFE_RELEASE(final_texture);
            SAFE_RELEASE(backbuffer_texture_surface);
            SAFE_RELEASE(backbuffer_texture);
            SAFE_RELEASE(dest_target);
            SAFE_RELEASE(render_target);
            return;
        }

        D3DLOCKED_RECT rect;
        dest_target->LockRect(&rect, nullptr, D3DLOCK_READONLY);
        std::memcpy(buffer, rect.pBits, width * height * 4);
        dest_target->UnlockRect();

        SAFE_RELEASE(final_texture_surface);
        SAFE_RELEASE(final_texture);
        SAFE_RELEASE(backbuffer_texture_surface);
        SAFE_RELEASE(backbuffer_texture);
    }
    else
    {
        // Copy render target to destination
        result = device->GetRenderTargetData(render_target, dest_target);

        if (FAILED(result))
        {
            SAFE_RELEASE(dest_target);
            SAFE_RELEASE(render_target);
            return;
        }

        D3DLOCKED_RECT rect;
        dest_target->LockRect(&rect, nullptr, D3DLOCK_READONLY);
        //std::memcpy(buffer, rect.pBits, width * height * 4);
        copy_image(buffer, rect.pBits, width, height, 4, image_format);
        dest_target->UnlockRect();
    }

    SAFE_RELEASE(dest_target);
    SAFE_RELEASE(render_target);
}

void dx_draw_point(IDirect3DDevice9* device, float cx, float cy, float radius, D3DCOLOR colour)
{
    static const int resolution = 10;
    D3DVertex Vertices[resolution];

    for (int i = 0; i < resolution; ++i)
    {
        Vertices[i].X = cx + radius * std::cos(3.141592654f * (static_cast<float>(i) / (resolution / 2.0f)));
        Vertices[i].Y = cy + radius * std::sin(3.141592654f * (static_cast<float>(i) / (resolution / 2.0f)));
        Vertices[i].Z = 0.0f;
        Vertices[i].RHW = 1.0f;
        Vertices[i].Colour = colour;
        Vertices[i].U = 0.0f;
        Vertices[i].V = 0.0f;
    }

    device->SetFVF(VERTEX_FVF_TEX);
    device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, resolution - 2, Vertices, sizeof(D3DVertex));
}
#endif
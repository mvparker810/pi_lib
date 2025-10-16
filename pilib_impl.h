// pilib_impl.h
#ifndef PILIB_IMPL_H
#define PILIB_IMPL_H

#include <stdint.h>
#include <stddef.h>

typedef uint16_t pilib_color;

typedef struct {
    uint8_t x;
    uint8_t y;
} pilib_vec2i;

typedef struct {
    int fb_fd;
    int width;
    int height;
    int bpp;
    size_t screensize;
    uint8_t *fb_mem;
    size_t line_length;
} PILIB_CONTEXT;

// === Colors ===
pilib_color pilib_colorFromRGB(uint8_t r, uint8_t g, uint8_t b);
pilib_color pilib_colorFromHEX(uint32_t code);
pilib_color pilib_colorFromHSV(float h, float s, float v);

// === Lifecycle ===
PILIB_CONTEXT* pilib_init(const char *fb_path);
void pilib_free(PILIB_CONTEXT *ctx);



// === Drawing ===
void pilib_drawing_clear(PILIB_CONTEXT *ctx, pilib_color clear_col);
void pilib_drawing_setPx(PILIB_CONTEXT *ctx, pilib_vec2i pos, pilib_color col);


// === Debug ===
void pilib_debug(PILIB_CONTEXT *ctx);

#endif // PILIB_IMPL_H

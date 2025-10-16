#define _GNU_SOURCE
#include <fcntl.h>
#include <linux/fb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <math.h>

#include "pilib_impl.h"



uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

pilib_color pilib_colorFromRGB8(uint8_t r, uint8_t g, uint8_t b) {
    return rgb565(r,g,b);
}
pilib_color pilib_colorFromHEX(uint32_t hex) {
    uint8_t r = (hex >> 16) & 0xFF;
    uint8_t g = (hex >> 8)  & 0xFF;
    uint8_t b = hex & 0xFF;
    return rgb565(r, g, b);
}

pilib_color pilib_colorFromHSV(float h, float s, float v) {
    while (h < 0.0f)   h += 360.0f;
    while (h >= 360.0f) h -= 360.0f;

    float c = v * s;
    float x = c * (1 - fabsf(fmodf(h / 60.0f, 2) - 1));
    float m = v - c;

    float r1, g1, b1;

    if      (h < 60.0f)  { r1 = c; g1 = x; b1 = 0; }
    else if (h < 120.0f) { r1 = x; g1 = c; b1 = 0; }
    else if (h < 180.0f) { r1 = 0; g1 = c; b1 = x; }
    else if (h < 240.0f) { r1 = 0; g1 = x; b1 = c; }
    else if (h < 300.0f) { r1 = x; g1 = 0; b1 = c; }
    else                { r1 = c; g1 = 0; b1 = x; }

    uint8_t r = (uint8_t)((r1 + m) * 255.0f);
    uint8_t g = (uint8_t)((g1 + m) * 255.0f);
    uint8_t b = (uint8_t)((b1 + m) * 255.0f);

    return rgb565(r, g, b);
}






PILIB_CONTEXT* pilib_init(const char *fb_path) {
    if (!fb_path) fb_path = "/dev/fb0";
    PILIB_CONTEXT *ctx = calloc(1, sizeof(PILIB_CONTEXT));
    if (!ctx) {
        perror("malloc");
        return NULL;
    }

    ctx->fb_fd = open(fb_path, O_RDWR);
    if (ctx->fb_fd < 0) {
        perror("open framebuffer");
        free(ctx);
        return NULL;
    }

    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;

    if (ioctl(ctx->fb_fd, FBIOGET_FSCREENINFO, &finfo) < 0) {
        perror("finfo");
        close(ctx->fb_fd);
        free(ctx);
        return NULL;
    }

    if (ioctl(ctx->fb_fd, FBIOGET_VSCREENINFO, &vinfo) < 0) {
        perror("vinfo");
        close(ctx->fb_fd);
        free(ctx);
        return NULL;
    }

    ctx->width  = vinfo.xres;
    ctx->height = vinfo.yres;
    ctx->bpp    = vinfo.bits_per_pixel;
    ctx->line_length = finfo.line_length;
    ctx->screensize = ctx->height * ctx->line_length;

    ctx->fb_mem = mmap(NULL, ctx->screensize, PROT_READ | PROT_WRITE, MAP_SHARED, ctx->fb_fd, 0);
    if (ctx->fb_mem == MAP_FAILED) {
        perror("mmap");
        close(ctx->fb_fd);
        free(ctx);
        return NULL;
    }

    return ctx;
}

void pilib_free(PILIB_CONTEXT *ctx) {
    if (!ctx) return;
    if (ctx->fb_mem && ctx->fb_mem != MAP_FAILED) {
        munmap(ctx->fb_mem, ctx->screensize);
    }
    if (ctx->fb_fd >= 0) {
        close(ctx->fb_fd);
    }
    free(ctx);
}

void pilib_drawing_clear(PILIB_CONTEXT *ctx, pilib_color clear_col) {
    if (!ctx || !ctx->fb_mem) return;

    for (int y = 0; y < ctx->height; y++) {
        for (int x = 0; x < ctx->width; x++) {
            size_t offset = y * ctx->line_length + x * (ctx->bpp / 8);
            *(uint16_t *)(ctx->fb_mem + offset) = clear_col;
        }
    }
}

void pilib_drawing_setPx(PILIB_CONTEXT *ctx, pilib_vec2i pos, pilib_color col) {
    if (!ctx || !ctx->fb_mem) return;
    if (pos.x >= ctx->width || pos.y >= ctx->height) return;

    size_t offset = pos.y * ctx->line_length + pos.x * (ctx->bpp / 8);
    *(uint16_t *)(ctx->fb_mem + offset) = col;
}

void pilib_debug(PILIB_CONTEXT *ctx) {
    printf("Framebuffer: %dx%d @ %d bpp\n", ctx->width, ctx->height, ctx->bpp);
    printf("Memory mapped: %zu bytes\n", ctx->screensize);
}

#include <stdio.h>
#include "pilib_impl.h"
#include <unistd.h>

int main() {
    PILIB_CONTEXT *ctx = pilib_init("/dev/fb0");
    if (!ctx) return 1;
    /*
    pilib_debug(ctx);

    pilib_clear(ctx, 0, 0, 0);
    sleep(1);

    pilib_fill_checkerboard(ctx);
    sleep(2);

    pilib_clear(ctx, 255, 0, 0); // red fill
    sleep(2);

    pilib_set_pixel(ctx, 3, 3, 0, 255, 0); // one green pixel
    sleep(2);
    */


    pilib_drawing_clear(ctx, 0x0);
    pilib_free(ctx);
    return 0;
}
#include "ppmCreator.h"

const int df_width = 128;
const int df_height = 128;
const int df_depth = 255;

int main() {
    writePPM("test_128_random", df_width, df_height, df_depth, [] (int offset) {
        return RGB((offset % 16) / 16.f, (offset / 128) / 128.f, (offset %  64) / 64.f );
        return createRandomPixel();
    });
    return 0;
}


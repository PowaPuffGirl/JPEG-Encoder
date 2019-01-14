#include "ppmCreator.h"

const int df_width = 16;
const int df_height = 16;
const int df_depth = 255;

int main() {
    writePPM("output", df_width, df_height, df_depth, [] (int offset) {
        return createRandomPixel();
    });
    return 0;
}


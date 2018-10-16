#include "ppmCreator.h"

const int df_width = 15;
const int df_height = 15;
const int df_depth = 65000;

int main() {
    writePPM("output", df_width, df_height, df_depth, [] (int offset) {
        return createRandomPixel();
    });
    return 0;
}


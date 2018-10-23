#include <iostream>
#include <fstream>
#include "ppmCreator.h"

void writePPM(std::string filename, int width, int height, int depth, std::function<RGB(int)> pixelSource) {
    std::string fileText = createHeaderOfPPM(width, height, depth);

    int offset = 0;
    for (int row = 0; row < height; row++) {
        for (int column = 0; column < width; column++) {
            RGB dt = pixelSource(offset++);
            fileText.append(std::to_string(
                    (int)(dt.red * depth)) + " " +
                    std::to_string((int)(dt.green * depth)) + " " +
                    std::to_string((int)(dt.blue * depth)) + "\t"
                );
        }
        fileText.append("\n");
    }

    writePPMFile(filename, fileText);
}

void writePPM(std::string filename, int width, int height, int depth, std::function<RGB(int, int)> pixelSource) {
    std::string fileText = createHeaderOfPPM(width, height, depth);

    for (int row = 0; row < height; row++) {
        for (int column = 0; column < width; column++) {
            RGB dt = pixelSource(column, row);
            fileText.append(
                    std::to_string(std::max(0, std::min((int)(dt.red * depth), depth))) + " " +
                    std::to_string(std::max(0, std::min((int)(dt.green * depth), depth))) + " " +
                    std::to_string(std::max(0, std::min((int)(dt.blue * depth), depth))) + "\t"
                );
        }
        fileText.append("\n");
    }

    writePPMFile(filename, fileText);
}

RGB createRandomPixel() {
    float red = (double) std::rand() / (RAND_MAX);
    float green = (double) std::rand() / (RAND_MAX);
    float blue = (double) std::rand() / (RAND_MAX);
    return RGB(red, green, blue);
}

void createPixelChangeingColorRows(std::string &fileText, int width, int height, int depth) {
    int red = depth;
    int green = 0;
    int blue = 0;
    for (int row = 0; row < height; row++) {
        for (int column = 0; column < width; column++) {
            fileText.append(std::to_string(red) + " " + std::to_string(green) + " " + std::to_string(blue) + "\t");
        }
        fileText.append("\n");
        if (red == depth) {
            green = depth;
            red = 0;
        } else if (green == depth) {
            blue = depth;
            green = 0;
        } else if (blue == depth) {
            red = depth;
            blue = 0;
        }
    }
}

void writePPMFile(const std::string &fileName, const std::string &fileText) {
    std::ofstream file;
    file.open ("../output/" + fileName + ".ppm");
    file << fileText;
    file.close();
}

std::string createHeaderOfPPM(int width, int height, int depth) {
    std::string header = std::string("");
    header.append("P3\n");
    header.append( std::to_string(width) + " " + std::to_string(height) + "\n");
    header.append( std::to_string(depth) + "\n");
    return header;
}
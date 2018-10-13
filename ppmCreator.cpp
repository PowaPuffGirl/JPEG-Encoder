#include <iostream>
#include <fstream>

const int width = 5;
const int height = 5;

std::string createHeaderOfPPM();
void writePPMFile(const std::string &fileName, const std::string &fileText);
void createPixelChangeingColorRows(std::string &fileText);
void createRandomPixels(std::string &fileText);

int main() {
    std::string fileText = createHeaderOfPPM();
    createRandomPixels(fileText);

    writePPMFile("test", fileText);
    std::cout << fileText << std::endl;
    return 0;
}

void createRandomPixels(std::string &fileText) {
    for (int row = 0; row < height; row++) {
        for (int column = 0; column < width; column++) {
            int red = (int) (((double) std::rand() / (RAND_MAX)) * 255);
            int green = (int) (((double) std::rand() / (RAND_MAX)) * 255);
            int blue = (int) (((double) std::rand() / (RAND_MAX)) * 255);
            fileText.append(std::to_string(red) + " " + std::to_string(green) + " " + std::to_string(blue) + "\t");
        }
        fileText.append("\n");
    }
}

void createPixelChangeingColorRows(std::string &fileText) {
    int red = 255;
    int green = 0;
    int blue = 0;
    for (int row = 0; row < height; row++) {
        for (int column = 0; column < width; column++) {
            fileText.append(std::to_string(red) + " " + std::to_string(green) + " " + std::to_string(blue) + "\t");
        }
        fileText.append("\n");
        if (red == 255) {
            green = 255;
            red = 0;
        } else if (green == 255) {
            blue = 255;
            green = 0;
        } else if (blue == 255) {
            red = 255;
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

std::string createHeaderOfPPM() {
    std::string header = std::string("");
    header.append("P3\n");
    header.append( std::to_string(width) + " " + std::to_string(height) + "\n");
    header.append("255\n");
    return header;
}
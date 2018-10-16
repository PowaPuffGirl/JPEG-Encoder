#ifndef MEDIENINFO_PPMCREATOR_H
#define MEDIENINFO_PPMCREATOR_H

#include <string>
#include <functional>
#include "PixelTypes.h"

std::string createHeaderOfPPM(int width, int height, int depth);
void writePPMFile(const std::string &fileName, const std::string &fileText);
void createPixelChangeingColorRows(std::string &fileText, int width, int height, int depth);
void writePPM(std::string filename, int width, int height, int depth, std::function<RGB(int)> pixelSource);
RGB createRandomPixel();
void createRandomPpm(std::string filename, int width, int height, int depth);

#endif //MEDIENINFO_PPMCREATOR_H

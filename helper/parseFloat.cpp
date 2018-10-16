#include <string>
#include "parseFloat.h"

void parseFloatSimple(char* str, int len, float* result) {
    std::string dt(str, len);
    *result = (float)atoi(dt.c_str());
}

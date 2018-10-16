#ifndef MEDIENINFO_YCBCR_H
#define MEDIENINFO_YCBCR_H

struct RGB;
struct YCBCR;

struct YCBCR {
    float y, cb, cr;
    YCBCR(const float y, const float cb, const float cr) {
        this->y = y;
        this->cb = cb;
        this-> cr = cr;
    }
};


struct RGB {
    int red, green, blue;
    RGB(const int red, const int green, const int blue) {
        this->red = red;
        this->green = green;
        this-> blue = blue;
    }

    YCBCR transformToYCBCR() {
        float a = 0.299f * this->red + 0.587f * this->green + 0.114f * this->blue;
        float b = -0.1687f * this->red + -0.3312f * this->green + 0.5f * this->blue + 0.5f;
        float c = 0.5f * this->red + -0.4186f * this->green + -0.0813f * this->blue + 0.5f;
        YCBCR result = YCBCR(a,b,c);
        return result;
    }
};


#endif //MEDIENINFO_YCBCR_H

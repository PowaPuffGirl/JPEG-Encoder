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
    float red, green, blue;

    RGB() : red(0), green(0), blue(0) {}
    RGB(float red, float green, float blue) : red(red), green(green), blue(blue) {}

    RGB fromYCbCr(const float Y, const float Cb, const float Cr) {
        this->red = (Y + 1.402f * (Cr -0.5f));
        this->green = (Y -0.344136f * (Cb -0.5f) -0.714135f * (Cr -0.5f));
        this->blue = (Y + 1.772f * (Cb -0.5f));
        return *this;
    }
};


#endif //MEDIENINFO_YCBCR_H

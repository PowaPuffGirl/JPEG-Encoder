#ifndef MEDIENINFO_RGBTOYCBCR_H
#define MEDIENINFO_RGBTOYCBCR_H

template <typename StorageType, int maxval>
void RgbToYCbCr(StorageType& r, StorageType& g, StorageType& b);


template <typename StorageType, int maxval>
inline void RgbToYCbCrFloatingPoint(StorageType& red, StorageType& green, StorageType& blue) {
    StorageType cb = ((-0.1687f * red) + (-0.3312f * green) + 0.5f * blue + (maxval * 0.5f)) - (maxval * 0.5f);
    StorageType cr = (0.5f * red + (-0.4186f * green) + (-0.0813f * blue) + (maxval * 0.5f)) - (maxval * 0.5f);
    red =            (0.299f * red + 0.587f * green + 0.114f * blue)                     - (maxval * 0.5f);
    green = cb;
    blue = cr;
}

template <int maxval>
void RgbToYCbCr(float& r, float& g, float& b) { RgbToYCbCrFloatingPoint<float, maxval>(r, g, b); }

template <int maxval>
void RgbToYCbCr(double& r, double& g, double& b) { RgbToYCbCrFloatingPoint<double, maxval>(r, g, b); }

/*
template <typename StorageType, int maxval>
void RgbToYCbCrInt(StorageType& red, StorageType& green, StorageType& blue) {
    // this is missing the substracted offset
    StorageType Cb = 128 + (-37.797 * red) + (-74.203 * green) + (112 * blue);
    StorageType Cr = 128 + (112 * red) + (-93.786 * green) + (-18.214 * blue);
    red = 16 + (65.481 * red) + (128.553 * green) + (24.996 * blue);
    green = Cb;
    blue = Cr;
}*/

#endif //MEDIENINFO_RGBTOYCBCR_H

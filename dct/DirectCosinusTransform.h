#ifndef MEDIENINFO_DISCRETECOSINTRANSFORM_H
#define MEDIENINFO_DISCRETECOSINTRANSFORM_H

#include <math.h>
#include <array>
#include "AbstractCosinusTransform.h"

template<typename T, unsigned int blocksize = 8>
class DirectCosinusTransform {
private:
    double C(int value) const {
        if (value == 0) {
            return 1/sqrt(2);
        } else {
            return 1;
        }
    }

    std::array<std::array<std::array<std::array<T, blocksize>, blocksize>, blocksize>, blocksize> coefficients;
    void buildBuffer() {
        for (unsigned int i = 0; i < blocksize; i++) {
            const double ci = ((i * M_PI) / (2. * blocksize));

            for (unsigned int j = 0; j < blocksize; j++) {
                const double cj = (j * M_PI) / (2. * blocksize);

                for (unsigned int x = 0; x < blocksize; x++) {
                    for (unsigned int y = 0; y < blocksize; y++) {
                        coefficients[x][y][i][j] =
                                (2./blocksize)*C(i)*C(j) * cos(((x << 1)+1)*ci) * cos(((y << 1) + 1) * cj);
                    }
                }

            }
        }
    }

    std::array<std::array<T, blocksize>, blocksize> sums = { 0 };
public:

    void transformBlock(const std::function<const T&(uint, uint)>& get, const std::function<T&(uint, uint)>& set) {
        sums = { 0 };

        for (unsigned int x = 0; x < blocksize; x++) {
            const auto& rowX = coefficients[x];
            for (unsigned int y = 0; y < blocksize; y++) {
                const auto& value = get(x, y);
                const auto& rowY = rowX[y];

                for (unsigned int i = 0; i < blocksize; i++) {
                    const auto& rowI = rowY[i];
                    for (unsigned int j = 0; j < blocksize; j++) {
                        sums[i][j] += value * rowI[j];
                    }
                }
            }
        }

        for (unsigned int i = 0; i < blocksize; i++) {
            for (unsigned int j = 0; j < blocksize; j++) {
                set(i,j) = sums[i][j];
            }
        }
    }

    DirectCosinusTransform() { buildBuffer(); }
};


#endif //MEDIENINFO_DISCRETECOSINTRANSFORM_H

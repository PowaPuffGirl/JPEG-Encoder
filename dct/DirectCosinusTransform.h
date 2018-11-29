#ifndef MEDIENINFO_DISCRETECOSINTRANSFORM_H
#define MEDIENINFO_DISCRETECOSINTRANSFORM_H

#include <math.h>
#include "AbstractCosinusTransform.h"

template<typename T, unsigned int blocksize = 8>
class DirectCosinusTransform : AbstractCosinusTransform<T> {
private:
    double C(int value) {
        if (value == 0) {
            return M_SQRT1_2;
        } else {
            return 1;
        }
    }

public:
    void transformChannel(const std::function<T&(uint, uint)>& get, const std::function<T&(uint, uint)>& set) override {
        for(unsigned int i = 0; i < blocksize; i++) {
            for(unsigned int j = 0; j < blocksize; j++) {
               double sum = 0;
                for (unsigned int x = 0; x < blocksize; x++) {
                    for (unsigned int y = 0; y < blocksize; y++) {
                        sum += get(x, y)*cos(((2*x+1)*i*M_PI)/2*blocksize) * cos(((2*y +1)*j*M_PI)/2*blocksize);
                     }
                }
                set(i,j) = 2/blocksize * C(i)*C(j)*sum;
            }
        }
    }

};


#endif //MEDIENINFO_DISCRETECOSINTRANSFORM_H
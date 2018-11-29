//
// Created by proj on 11/29/18.
//

#ifndef MEDIENINFO_SEPARATEDCOSINUSTRANSFORMGLM_H
#define MEDIENINFO_SEPARATEDCOSINUSTRANSFORMGLM_H


#include "AbstractCosinusTransform.h"
#include <glm/matrix.hpp>
#include <glm/mat4x4.hpp>


template<typename T>
class SeparatedCosinusTransform : public AbstractCosinusTransform<T> {
private:
    using mat8x8 = glm::mat<8, 8, T, glm::defaultp>;

    double C(int n) {
        if(n == 0) {
            return 1./sqrt(2);
        }
        return 1.;
    }

    mat8x8 generateA() {
        mat8x8 A;
        const T N = 8*8;
        const double prefix = sqrt(2./N);
        const double suffix = M_PI_2 * N;

        for (int k = 0; k < 8; ++k) {
            for (int n = 0; n < 8; ++n) {
                A(k, n) = C(k) * prefix * cos(((n << 1) + 1) * k * suffix);
            }
        }

        return A;
    }

    mat8x8 generateAT() {
        return glm::transpose(generateA());
    }

public:
    virtual void transformChannel(const std::vector<T>& channel, std::vector<T>& output, int xsize, int ysize) {
        assert(channel.size() % (8*8) == 0);
    }

};

#endif //MEDIENINFO_SEPARATEDCOSINUSTRANSFORMGLM_H

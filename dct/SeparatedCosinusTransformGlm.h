//
// Created by proj on 11/29/18.
//

#ifndef MEDIENINFO_SEPARATEDCOSINUSTRANSFORMGLM_H
#define MEDIENINFO_SEPARATEDCOSINUSTRANSFORMGLM_H


#include "AbstractCosinusTransform.h"
#include <glm/matrix.hpp>
#include <glm/mat4x4.hpp>


template<typename T, unsigned int blocksize = 8>
class SeparatedCosinusTransform : public AbstractCosinusTransform<T, blocksize> {
private:
    using mat8x8 = glm::mat<blocksize, blocksize, T, glm::defaultp>;

    double C(int n) {
        if(n == 0) {
            return 1./sqrt(2);
        }
        return 1.;
    }

    mat8x8 generateA() {
        mat8x8 A;
        const T N = blocksize;
        const double prefix = sqrt(2./N);
        const double suffix = M_PI_2 * N;

        for (uint k = 0; k < blocksize; ++k) {
            for (uint n = 0; n < blocksize; ++n) {
                A(k, n) = C(k) * prefix * cos(((n << 1) + 1) * k * suffix);
            }
        }

        return A;
    }

    mat8x8 generateAT() {
        return glm::transpose(generateA());
    }

    void generateMatFromChannel(const std::function<T&(uint, uint)>& get, mat8x8& matrix) {
        for (uint y = 0; y < blocksize; ++y) {
            for (uint x = 0; x < blocksize; ++x) {
                matrix(x, y) = get(x, y);
            }
        }
    }

    void writeMat(const std::function<T&(uint, uint)>& set, mat8x8& matrix) {
        for (uint y = 0; y < blocksize; ++y) {
            for (uint x = 0; x < blocksize; ++x) {
                set(x, y) = matrix(x, y);
            }
        }
    }

public:
    void transformBlock(const std::function<T&(uint, uint)>& get, const std::function<T&(uint, uint)>& set) {
        mat8x8 X;
        generateMatFromChannel(get, X);
        mat8x8 Y = generateA() * X * generateAT();
        writeMat(set, Y);
    }
};

#endif //MEDIENINFO_SEPARATEDCOSINUSTRANSFORMGLM_H

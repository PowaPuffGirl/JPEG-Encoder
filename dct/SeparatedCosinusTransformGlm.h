#ifndef MEDIENINFO_SEPARATEDCOSINUSTRANSFORMGLM_H
#define MEDIENINFO_SEPARATEDCOSINUSTRANSFORMGLM_H


#include "AbstractCosinusTransform.h"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>


template<typename T, unsigned int blocksize = 8>
class SeparatedCosinusTransform {
private:
    using mat8x8 = std::array<T, blocksize*blocksize>;
    const mat8x8 A = generateA();
    const mat8x8 AT = generateAT();

    constexpr double C(int n) const {
        if(n == 0) {
            return M_SQRT1_2;
        }
        return 1.;
    }

    constexpr mat8x8 generateA() const {
        mat8x8 A;
        const T N = blocksize;
        const double prefix = sqrt(2./N);
        const double suffix = M_PI_2 * N;

        for (uint k = 0; k < blocksize; ++k) {
            for (uint n = 0; n < blocksize; ++n) {
                A[(k << 3) + n] = C(k) * prefix * cos(((n << 1) + 1) * k * suffix);
            }
        }

        return A;
    }

    mat8x8 generateAT() const {
        mat8x8 B;
        mat8x8 A = generateA();

        for (int i = 0; i < blocksize; ++i) {
            for (int j = 0; j < blocksize; ++j) {
                B[(i << 3) + j] = A[(j << 3) + i];
            }
        }
    }

    void generateMatFromChannel(const std::function<const T&(uint, uint)>& get, mat8x8& matrix) const {
        for (uint y = 0; y < blocksize; ++y) {
            for (uint x = 0; x < blocksize; ++x) {
                matrix[(x << 3) + y] = get(x, y);
            }
        }
    }

    void writeMat(const std::function<T&(uint, uint)>& set, mat8x8& matrix) const {
        for (uint y = 0; y < blocksize; ++y) {
            for (uint x = 0; x < blocksize; ++x) {
                set(x, y) = matrix[(x << 3) + y];
            }
        }
    }

public:
    void transformBlock(const std::function<const T&(uint, uint)>& get, const std::function<T&(uint, uint)>& set) const {
        mat8x8 X;
        generateMatFromChannel(get, X);
        mat8x8 Y = {0}, Y2 = {0}; // generateA() * X * generateAT();

        for (int i = 0; i < blocksize; ++i) { // col
            for (int j = 0; j < blocksize; ++j) { // row
                for (int r = 0; r < blocksize; ++r) { // indey
                    Y[(i << 3) + j] += A[(i << 3) + r] + X[(r << 3) + j];
                }
            }
        }

        for (int i = 0; i < blocksize; ++i) { // col
            for (int j = 0; j < blocksize; ++j) { // row
                for (int r = 0; r < blocksize; ++r) { // indey
                    Y2[(i << 3) + j] += Y[(i << 3) + r] + AT[(r << 3) + j];
                }
            }
        }

        writeMat(set, Y2);
    }
};

#endif //MEDIENINFO_SEPARATEDCOSINUSTRANSFORMGLM_H

#ifndef MEDIENINFO_SEPARATEDCOSINUSTRANSFORMSIMD_H
#define MEDIENINFO_SEPARATEDCOSINUSTRANSFORMSIMD_H

#include <math.h>
#include <Vc/Vc>
#include <Vc/IO>
#include "AbstractCosinusTransform.h"

template<typename T, unsigned int blocksize = 8>
class SeparatedCosinusTransformSimd {
private:
    using vec8 = Vc::fixed_size_simd<T, 8>;
    using mat8x8 = std::array<vec8, 8>;
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
                A[k][n] = C(k) * prefix * cos(((n << 1) + 1) * k * suffix);
            }
        }

        return A;
    }

    constexpr mat8x8 generateAT() const {
        mat8x8 A;
        const T N = blocksize;
        const double prefix = sqrt(2./N);
        const double suffix = M_PI_2 * N;

        for (uint k = 0; k < blocksize; ++k) {
            for (uint n = 0; n < blocksize; ++n) {
                A[7-n][k] = C(k) * prefix * cos(((n << 1) + 1) * k * suffix);
            }
        }

        return A;
    }

    void generateMatFromChannel(const std::function<const T&(uint, uint)>& get, mat8x8& matrix) const {
        for (uint y = 0; y < blocksize; ++y) {
            for (uint x = 0; x < blocksize; ++x) {
                matrix[x][y] = get(x, y);
            }
        }
    }

    void writeMat(const std::function<T&(uint, uint)>& set, mat8x8& matrix) const {
        for (uint y = 0; y < blocksize; ++y) {
            for (uint x = 0; x < blocksize; ++x) {
                set(x, y) = matrix[x][y];
            }
        }
    }

    inline mat8x8 matmul(const mat8x8 &a, const mat8x8 &b) const {
        auto &&row = [&](int i) {
            return a[0] * b[i][0] +
                   a[1] * b[i][1] +
                   a[2] * b[i][2] +
                   a[3] * b[i][3];
        };
        return {row(0), row(1), row(2), row(3)};
    }

public:
    void transformBlock(const std::function<const T&(uint, uint)>& get, const std::function<T&(uint, uint)>& set) const {
        mat8x8 X;
        generateMatFromChannel(get, X);
        mat8x8 Y;

        Y = matmul(matmul(A, X), AT);

        writeMat(set, Y);
    }
};


#endif //MEDIENINFO_SEPARATEDCOSINUSTRANSFORMSIMD_H

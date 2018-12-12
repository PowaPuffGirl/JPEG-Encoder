#ifndef MEDIENINFO_SEPARATEDCOSINUSTRANSFORMGLM_H
#define MEDIENINFO_SEPARATEDCOSINUSTRANSFORMGLM_H


#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/operation_blocked.hpp>
#include "AbstractCosinusTransform.h"


template<typename T, unsigned int blocksize = 8>
class SeparatedCosinusTransform {
private:
    using mat8x8 = boost::numeric::ublas::matrix<T>;
    const mat8x8 A = generateA();
    const mat8x8 AT = generateAT();


    constexpr double C(int n) const {
        if(n == 0) {
            return 1/sqrt(2);
        }
        return 1.;
    }

    constexpr mat8x8 generateA() const {
        mat8x8 A(8, 8);
        const T N = blocksize;
        const double prefix = sqrt(2./N);

        for (uint k = 0; k < blocksize; ++k) {
            for (uint n = 0; n < blocksize; ++n) {
                A(k, n) = C(k) * prefix * cos(((n << 1) + 1) * (k * M_PI) / (2*N) );
            }
        }

        return A;
    }

    constexpr mat8x8 generateAT() const {
        return boost::numeric::ublas::trans(generateA());
    }

    void generateMatFromChannel(const std::function<const T&(uint, uint)>& get, mat8x8& matrix) const {
        for (uint y = 0; y < blocksize; ++y) {
            for (uint x = 0; x < blocksize; ++x) {
                matrix(x, y) = get(x, y);
            }
        }
    }

    void writeMat(const std::function<T&(uint, uint)>& set, mat8x8& matrix) const {
        for (uint y = 0; y < blocksize; ++y) {
            for (uint x = 0; x < blocksize; ++x) {
                set(x, y) = matrix(x, y);
            }
        }
    }

public:
    void transformBlock(const std::function<const T&(uint, uint)>& get, const std::function<T&(uint, uint)>& set) const {
        using namespace boost::numeric::ublas;

        mat8x8 X(8,8);
        generateMatFromChannel(get, X);
        mat8x8 Y(8,8);

        //auto Y1 = block_prod<matrix<T>, 1024>(A, X);
        Y = block_prod<matrix<T>, 1024>(block_prod<matrix<T>, 1024>(A, X), AT);

        writeMat(set, Y);
    }
};

#endif //MEDIENINFO_SEPARATEDCOSINUSTRANSFORMGLM_H

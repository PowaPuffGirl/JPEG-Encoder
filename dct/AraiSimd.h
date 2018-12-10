#ifndef MEDIENINFO_ARAISIMD_H
#define MEDIENINFO_ARAISIMD_H

#include "AbstractCosinusTransform.h"
#include <Vc/Vc>
#include <Vc/IO>


template<typename T, unsigned int blocksize = 8>
class AraiSimd {
private:
    using vec8 = Vc::fixed_size_simd<T, blocksize>;
    using rowBlock = std::array<vec8, blocksize>;

    static T C(T k) {
        return cos((k * M_PI) / 16);
    }

    static constexpr T sc(T k) {
        return 1/(4*C(k));
    }

    std::array<T, 6> a = {
            0,
            C(4),
            C(2) - C(6),
            C(4),
            C(6) + C(2),
            C(6)
    };


    std::array<T, 8> s = {
        M_SQRT1_2 * 0.5,
        sc(1),
        sc(2),
        sc(3),
        sc(4),
        sc(5),
        sc(6),
        sc(7)
    };

    inline void getRowBlocks(rowBlock& vert, rowBlock& hor, const std::function<const T&(uint, uint)>& get) const {
        for(uint x = 0; x < blocksize; ++x) {
            for (uint y = 0; y < blocksize; ++y) {
                const auto& val = get(x, y);
                hor[y][x] = val;
                vert[x][y] = val;
            }
        }
    }

    inline void setRowBlocks(rowBlock& vert, rowBlock& hor, const std::function<T&(uint, uint)>& set) const {
        for(uint x = 0; x < blocksize; ++x) {
            for (uint y = 0; y < blocksize; ++y) {
                set(x, y) = hor[y][x] + vert[x][y];
            }
        }
    }

    void arai(rowBlock& row) const {
        auto& x0 = row[0];
        auto& x1 = row[1];
        auto& x2 = row[2];
        auto& x3 = row[3];
        auto& x4 = row[4];
        auto& x5 = row[5];
        auto& x6 = row[6];
        auto& x7 = row[7];

        vec8 x0c = x0 + x7;
        vec8 x1c = x1 + x6;
        vec8 x2c = x2 + x5;
        vec8 x3c = x3 + x4;
        x4 = -x4 + x3;
        x5 = -x5 + x2;
        x6 = -x6 + x1;
        x7 = -x7 + x0;

        x3 = x0c - x3c;
        x2 = x1c - x2c;
        x1c = x1c + x2c;
        x0 = x0c + x3c;
        x4 = -x4 - x5;
        x5 += x6;
        x6 += x7;
        // x7 = x7

        x1 = (-x1c + x0) * s[4];
        x0 = (x0 * x1c) * s[0];
        x2c = (x2 + x3) * a[1];
        x2 = (x2c + x3) * s[2];
        x3 = (x3 - x2c) * s[6];

        // alias to save memory
#define a5c x0c
        a5c = (x4 + x6) * a[5];
        x4 = (-a[2] * x4) - a5c;
        x5 *= a[3];
        x6 = (a[4] * x6) - a5c;
        // x7 = x7

#define x5c x0c
#define x7c x1c
        x5c = x5 + x7;
        x7c -= x5;

        x7 = (x7c - x4) * s[3];
        x5 = (x5c + x6) * s[1];
        x4 = (x7c + x4) * s[5];
        x6 = (x5c - x6) * s[7];

        x0c = x1;
        x1 = x4;
        x4 = x5;
        x5 = x0c;

        x0c = x3;
        x3 = x6;
        x6 = x7;
        x7 = x0c;
    }

    rowBlock hor, vert;
public:
    void transformBlock(const std::function<const T&(uint, uint)>& get, const std::function<T&(uint, uint)>& set) {
        getRowBlocks(vert, hor, get);
        arai(hor);
        arai(vert);
        setRowBlocks(vert, hor, set);
    }
};


#endif //MEDIENINFO_ARAISIMD_H

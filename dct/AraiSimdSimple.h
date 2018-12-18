#ifndef MEDIENINFO_ARAISIMDSIMPLE_H
#define MEDIENINFO_ARAISIMDSIMPLE_H

#include "AbstractCosinusTransform.h"
#include "AraiCosinusTransform.h"
#include <Vc/Vc>
#include <Vc/IO>

template<typename T, unsigned int blocksize = 8>
class AraiSimdSimple {
    using vec8 = Vc::fixed_size_simd<T, blocksize>;
    using rowBlock = std::array<vec8, blocksize>;

    std::array<T, 6> a = {
            0,
            a1,
            a2,
            a3,
            a4,
            a5
    };


    std::array<T, 8> s = {
            s0,
            s1,
            s2,
            s3,
            s4,
            s5,
            s6,
            s7
    };


public:
    void transformBlock(const std::function<const T &(uint, uint)>& get, const std::function<T &(uint, uint)>& set) const {
        vec8 y0,y1,y2,y3,y4,y5,y6,y7 = 0;
        vec8 ty0, ty1, ty2, ty3, ty4, ty5, ty6, ty7 = 0;
        for(uint i = 0; i < 8; i++) {
            y0[i] = get(i,0) + get(i,7);
            y1[i] = get(i,1) + get(i,6);
            y2[i] = get(i,2) + get(i,5);
            y3[i] = get(i,3) + get(i,4);
            y4[i] = get(i,3) - get(i,4);
            y5[i] = get(i,2) - get(i,5);
            y6[i] = get(i,1) - get(i,6);
            y7[i] = get(i,0) - get(i,7);

        }

        ty0 = y0 + y3;
        ty1 = y1 + y2;
        ty2 = y1 - y2;
        ty3 = y0 - y3;
        ty4 = y4 + y5;
        ty5 = y5 + y6;
        ty6 = y6 + y7;

        y1 = ty0 - ty1;
        ty0 = (ty0 + ty1) * s[0];
        ty2 = (ty2 + ty3) * a[1];
        y4 = ty4;

        ty4 = y4*a[2] - ((ty6-y4)*a[5]);
        ty6 = ty6*a[4] - ((ty6+y4)*a[5]);

        y2 = ty2 + ty3;
        ty3 = (ty3 - ty2) * s[6];
        y5 = (ty5 * a[3]) + y7;
        y7 = y7 - ty5;

        ty1 = y1 * s[4];
        ty2 = y2 * s[2];
        ty7 = (y7 - ty4) * s[3];
        ty4 = (ty4 + y7) * s[5];
        ty5 = (y5 + ty6) * s[1];
        ty6 = (y5 - ty6) * s[7];

        std::array<vec8*, 8> rb = {
                &ty0,
                &ty5,
                &ty2,
                &ty7,
                &ty1,
                &ty4,
                &ty3,
                &ty6
        };
        for(uint i = 0; i < 8; i++) {
            const auto& c = *rb[i];
            y0[i] = c[0] + c[7];
            y1[i] = c[1] + c[6];
            y2[i] = c[2] + c[5];
            y3[i] = c[3] + c[4];
            y4[i] = c[3] - c[4];
            y5[i] = c[2] - c[5];
            y6[i] = c[1] - c[6];
            y7[i] = c[0] - c[7];
        }

        ty0 = y0 + y3;
        ty1 = y1 + y2;
        ty2 = y1 - y2;
        ty3 = y0 - y3;
        ty4 = y4 + y5;
        ty5 = y5 + y6;
        ty6 = y6 + y7;

        y1 = ty0 - ty1;
        ty0 = (ty0 + ty1) * s[0];
        ty2 = (ty2 + ty3) * a[1];
        y4 = ty4;

        ty4 = y4*a[2] - ((ty6-y4)*a[5]);
        ty6 = ty6*a[4] - ((ty6+y4)*a[5]);

        y2 = ty2 + ty3;
        ty3 = (ty3 - ty2) * s[6];
        y5 = (ty5 * a[3]) + y7;
        y7 = y7 - ty5;

        ty1 = y1 * s[4];
        ty2 = y2 * s[2];
        ty7 = (y7 - ty4) * s[3];
        ty4 = (ty4 + y7) * s[5];
        ty5 = (y5 + ty6) * s[1];
        ty6 = (y5 - ty6) * s[7];

        for(uint i = 0; i < 8; i++) {
            set(0,i) = ty0[i];
            set(4,i) = ty1[i];
            set(2,i) = ty2[i];
            set(6,i) = ty3[i];
            set(5,i) = ty4[i];
            set(1,i) = ty5[i];
            set(7,i) = ty6[i];
            set(3,i) = ty7[i];
        }
    }
};


#endif //MEDIENINFO_ARAISIMDSIMPLE_H

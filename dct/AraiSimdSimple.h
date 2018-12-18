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
        ty7 = y7;

        y0 = ty0 + ty1;
        y1 = ty0 - ty1;
        y2 = ty2 + ty3;
        y3 = ty3;
        y4 = ty4;
        y5 = ty5;
        y6 = ty6;
        y7 = ty7;

        ty0 = y0;
        ty1 = y1;
        ty2 = y2 * a1;
        ty3 = y3;
        ty4 = y4*a2 - ((y6-y4)*a5);
        ty5 = y5 * a3;
        ty6 = y6*a4 - ((y6+y4)*a5);
        ty7 = y7;

        y0 = ty0;
        y1 = ty1;
        y2 = ty2 + ty3;
        y3 = ty3 - ty2;
        y4 = ty4;
        y5 = ty5 + ty7;
        y6 = ty6;
        y7 = ty7 - ty5;

        ty0 = y0 * s0;
        ty1 = y1 * s4;
        ty2 = y2 * s2;
        ty3 = y3 * s6;
        ty4 = (y4 + y7) * s5;
        ty5 = (y5 + y6) * s1;
        ty6 = (y5 - y6) * s7;
        ty7 = (y7 - y4) * s3;

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
            /*
            set(i, 0) = ty0[i];
            set(i, 1) = ty5[i];
            set(i, 2) = ty2[i];
            set(i, 3) = ty7[i];
            set(i, 4) = ty1[i];
            set(i, 5) = ty4[i];
            set(i, 6) = ty3[i];
            set(i, 7) = ty6[i];
            */

            const auto& c = *rb[i];
            y0[i] = c[0] + c[7];
            y1[i] = c[1] + c[6];
            y2[i] = c[2] + c[5];
            y3[i] = c[3] + c[4];
            y4[i] = c[3] - c[4];
            y5[i] = c[2] - c[5];
            y6[i] = c[1] - c[6];
            y7[i] = c[0] - c[7];

            /*
            y0 = set(0,i) + set(7,i);
            y1 = set(1,i) + set(6,i);
            y2 = set(2,i) + set(5,i);
            y3 = set(3,i) + set(4,i);
            y4 = set(3,i) - set(4,i);
            y5 = set(2,i) - set(5,i);
            y6 = set(1,i) - set(6,i);
            y7 = set(0,i) - set(7,i);
             */
        }


        //double y0,y1,y2,y3,y4,y5,y6,y7 = 0;
        //double ty0, ty1, ty2, ty3, ty4, ty5, ty6, ty7 = 0;

        ty0 = y0 + y3;
        ty1 = y1 + y2;
        ty2 = y1 - y2;
        ty3 = y0 - y3;
        ty4 = y4 + y5;
        ty5 = y5 + y6;
        ty6 = y6 + y7;
        ty7 = y7;

        y0 = ty0 + ty1;
        y1 = ty0 - ty1;
        y2 = ty2 + ty3;
        y3 = ty3;
        y4 = ty4;
        y5 = ty5;
        y6 = ty6;
        y7 = ty7;

        ty0 = y0;
        ty1 = y1;
        ty2 = y2 * a1;
        ty3 = y3;
        ty4 = y4*a2 - ((y6-y4)*a5);
        ty5 = y5 * a3;
        ty6 = y6*a4 - ((y6+y4)*a5);
        ty7 = y7;

        y0 = ty0;
        y1 = ty1;
        y2 = ty2 + ty3;
        y3 = ty3 - ty2;
        y4 = ty4;
        y5 = ty5 + ty7;
        y6 = ty6;
        y7 = ty7 - ty5;

        ty0 = y0;
        ty1 = y1;
        ty2 = y2;
        ty3 = y3;
        ty4 = y4 + y7;
        ty5 = y5 + y6;
        ty6 = y5 - y6;
        ty7 = y7 - y4;

        for(uint i = 0; i < 8; i++) {
            set(0,i) = ty0[i] * s0;
            set(4,i) = ty1[i] * s4;
            set(2,i) = ty2[i] * s2;
            set(6,i) = ty3[i] * s6;
            set(5,i) = ty4[i] * s5;
            set(1,i) = ty5[i] * s1;
            set(7,i) = ty6[i] * s7;
            set(3,i) = ty7[i] * s3;
        }
    }
};


#endif //MEDIENINFO_ARAISIMDSIMPLE_H

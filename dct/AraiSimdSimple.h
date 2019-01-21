#ifndef MEDIENINFO_ARAISIMDSIMPLE_H
#define MEDIENINFO_ARAISIMDSIMPLE_H

#include <Vc/Vc>
#include <Vc/IO>
#include <math.h>
#include "AbstractCosinusTransform.h"

#define a1 0.707106781186547524400844362104849039284835937688474036588
#define a2 0.541196100146196984399723205366389420061072063378015444681
#define a3 0.707106781186547524400844362104849039284835937688474036588
#define a4 1.306562964876376527856643173427187153583761188349269527548
#define a5 0.382683432365089771728459984030398866761344562485627041433

#define s0 0.353553390593273762200422181052424519642417968844237018294
#define s1 0.254897789552079584470969901993921956841309245954467684863
#define s2 0.270598050073098492199861602683194710030536031689007722340
#define s3 0.300672443467522640271860911954610917533627944800336361060
#define s4 0.353553390593273762200422181052424519642417968844237018294
#define s5 0.449988111568207852319254770470944197769000863706422492617
#define s6 0.653281482438188263928321586713593576791880594174634763774
#define s7 1.281457723870753089398043148088849954507561675693672456063

template<typename T, unsigned int blocksize = 8>
class AraiSimdSimple {
private:
    using vec8 = Vc::fixed_size_simd<T, blocksize>;
    using rowBlock = std::array<vec8, blocksize>;

    std::array<T, 6> a = {
            0,
            FixedPointConverter<T>::convert(a1),
            FixedPointConverter<T>::convert(a2),
            FixedPointConverter<T>::convert(a3),
            FixedPointConverter<T>::convert(a4),
            FixedPointConverter<T>::convert(a5)
    };


    std::array<T, 8> s = {
            FixedPointConverter<T>::convert(s0),
            FixedPointConverter<T>::convert(s1),
            FixedPointConverter<T>::convert(s2),
            FixedPointConverter<T>::convert(s3),
            FixedPointConverter<T>::convert(s4),
            FixedPointConverter<T>::convert(s5),
            FixedPointConverter<T>::convert(s6),
            FixedPointConverter<T>::convert(s7)
    };

    vec8 y0,y1,y2,y3,y4,y5,y6,y7 = 0;
    vec8 ty0, ty1, ty2, ty3, ty4, ty5, ty6, ty7 = 0;
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

    std::array<vec8*, 8> rb_ordered = {
            &ty0,
            &ty1,
            &ty2,
            &ty3,
            &ty4,
            &ty5,
            &ty6,
            &ty7
    };

public:
    void transformBlock(const std::function<const T &(uint, uint)>& get, const std::function<T &(uint, uint)>& set) {
        for(uint i = 0; i < 8; i++) {
            y0[i] = get(i,0);
            y1[i] = get(i,1);
            y2[i] = get(i,2);
            y3[i] = get(i,3);
            ty4[i] = get(i,4);
            ty5[i] = get(i,5);
            ty6[i] = get(i,6);
            ty7[i] = get(i,7);
        }

        y4 = y3 - ty4;
        y5 = y2 - ty5;
        y6 = y1 - ty6;
        y7 = y0 - ty7;
        y0 += ty7;
        y1 += ty6;
        y2 += ty5;
        y3 += ty4;



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

    template<typename CoordType>
    void transformBlock(rowBlock& block, const std::function<void (const CoordType, const CoordType, const T)>& set) {

        vec8& y0 = block[0];
        vec8& y1 = block[1];
        vec8& y2 = block[2];
        vec8& y3 = block[3];
        vec8& y4 = block[4];
        vec8& y5 = block[5];
        vec8& y6 = block[6];
        vec8& y7 = block[7];

        ty4 = y4;
        ty5 = y5;
        ty6 = y6;
        ty7 = y7;

        y4 = y3 - ty4;
        y5 = y2 - ty5;
        y6 = y1 - ty6;
        y7 = y0 - ty7;
        y0 += ty7;
        y1 += ty6;
        y2 += ty5;
        y3 += ty4;



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

        for(CoordType i = 0; i < 8; i++) {
            set(i, 0, ty0[i]);
            set(i, 4, ty1[i]);
            set(i, 2, ty2[i]);
            set(i, 6, ty3[i]);
            set(i, 5, ty4[i]);
            set(i, 1, ty5[i]);
            set(i, 7, ty6[i]);
            set(i, 3, ty7[i]);
        }
    }
};


#endif //MEDIENINFO_ARAISIMDSIMPLE_H

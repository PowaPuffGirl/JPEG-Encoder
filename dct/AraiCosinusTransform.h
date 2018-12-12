#ifndef MEDIENINFO_ARAICOSINTRANSFORM_H
#define MEDIENINFO_ARAICOSINTRANSFORM_H

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

#include <math.h>
#include "AbstractCosinusTransform.h"

template<typename T>
class AraiCosinusTransform {
public:
    void transformBlock(const std::function<const T &(uint, uint)>& get, const std::function<T &(uint, uint)>& set) const {
        for(uint i = 0; i < 8; i++) {
            double y0,y1,y2,y3,y4,y5,y6,y7 = 0;
            double ty0, ty1, ty2, ty3, ty4, ty5, ty6, ty7 = 0;
            y0 = get(i,0) + get(i,7);
            y1 = get(i,1) + get(i,6);
            y2 = get(i,2) + get(i,5);
            y3 = get(i,3) + get(i,4);
            y4 = get(i,3) - get(i,4);
            y5 = get(i,2) - get(i,5);
            y6 = get(i,1) - get(i,6);
            y7 = get(i,0) - get(i,7);

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

            set(i, 0) = ty0 * s0;
            set(i, 4) = ty1 * s4;
            set(i, 2) = ty2 * s2;
            set(i, 6) = ty3 * s6;
            set(i, 5) = ty4 * s5;
            set(i, 1) = ty5 * s1;
            set(i, 7) = ty6 * s7;
            set(i, 3) = ty7 * s3;
        }
        for(uint i = 0; i < 8; i++) {
            double y0,y1,y2,y3,y4,y5,y6,y7 = 0;
            double ty0, ty1, ty2, ty3, ty4, ty5, ty6, ty7 = 0;
            y0 = set(0,i) + set(7,i);
            y1 = set(1,i) + set(6,i);
            y2 = set(2,i) + set(5,i);
            y3 = set(3,i) + set(4,i);
            y4 = set(3,i) - set(4,i);
            y5 = set(2,i) - set(5,i);
            y6 = set(1,i) - set(6,i);
            y7 = set(0,i) - set(7,i);

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

            set(0,i) = ty0 * s0;
            set(4,i) = ty1 * s4;
            set(2,i) = ty2 * s2;
            set(6,i) = ty3 * s6;
            set(5,i) = ty5 * s5;
            set(1,i) = ty5 * s1;
            set(7,i) = ty6 * s7;
            set(3,i) = ty7 * s3;
        }
    }

    void transformBlock1(const std::function<const T &(uint, uint)>& get, const std::function<T &(uint, uint)>& set) const {
        for(uint i = 0; i < 8; i++) {
            double s07 = get(i,0) + get(i,7);
            double s16 = get(i,1) + get(i,6);
            double s25 = get(i,2) + get(i,5);
            double s34 = get(i,3) + get(i,4);

            double d07 = get(i,0) - get(i,7);
            double d16 = get(i,1) - get(i,6);
            double d25 = get(i,2) - get(i,5);
            double d34 = get(i,3) - get(i,4);

            double m1 = 2 * ((s07 + s34) + (s25+s16));
            double m2 = (s07 +s34) - (s25 +s16);
            double m3 = (s07 - s34);
            double m4 = d07;
            double m5 = a1 * ((s16 - s25) + (s07 - s34));
            double m6 = a1 * (d25 + d16);
            double m7 = a5 * ((d16 + d07) - (d25 + d34));
            double m8 = a4 * (d16 + d07);
            double m9 = a2 * (d25 + d34);

            double se5 = m4 + m6;
            double se6 = m4 - m6;
            double se7 = m8 - m7;
            double se8 = m9 - m7;

            set(i,0) = m1 * s0;
            set(i,4) = m2 * s1;
            set(i,2) = (m3 + m5) * s2;
            set(i,6) = (m3 - m5) * s3;
            set(i,3) = (se6 - se8) * s4;
            set(i,5) = (se6 + se8) * s5;
            set(i,1) = (se5 + se7) * s6;
            set(i,7) = (se5 - se7) * s7;
        }
        for(uint i = 0; i < 8; i++) {
            double s07 = set(0,i) + set(7,i);
            double s16 = set(1,i) + set(6,i);
            double s25 = set(2,i) + set(5,i);
            double s34 = set(3,i) + set(4,i);

            double d07 = set(0,i) - set(7,i);
            double d16 = set(1,i) - set(6,i);
            double d25 = set(2,i) - set(5,i);
            double d34 = set(3,i) - set(4,i);

            double m1 = 2 * ((s07 + s34) + (s25+s16));
            double m2 = (s07 +s34) - (s25 +s16);
            double m3 = (s07 - s34);
            double m4 = d07;
            double m5 = a1 * ((s16 - s25) + (s07 - s34));
            double m6 = a1 * (d25 + d16);
            double m7 = a5 * ((d16 + d07) - (d25 + d34));
            double m8 = a4 * (d16 + d07);
            double m9 = a2 * (d25 + d34);

            double se5 = m4 + m6;
            double se6 = m4 - m6;
            double se7 = m8 - m7;
            double se8 = m9 - m7;

            set(0,i) = m1 * s0;
            set(4,i) = m2 * s1;
            set(2,i) = (m3 + m5) * s2;
            set(6,i) = (m3 - m5) * s3;
            set(3,i) = (se6 - se8) * s4;
            set(5,i) = (se6 + se8) * s5;
            set(1,i) = (se5 + se7) * s6;
            set(7,i) = (se5 - se7) * s7;
        }
    }
};

#endif //MEDIENINFO_ARAICOSINTRANSFORM_H

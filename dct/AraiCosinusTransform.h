#ifndef MEDIENINFO_ARAICOSINTRANSFORM_H
#define MEDIENINFO_ARAICOSINTRANSFORM_H

#include <math.h>
#include "AbstractCosinusTransform.h"

const double a1 = cos(4*M_PI/16);
const double a2 = cos(2*M_PI/16) - cos(6*M_PI/16);
const double a3 = cos(4*M_PI/16);
const double a4 = cos(6*M_PI/16) + cos(2*M_PI/16);
const double a5 = cos(6*M_PI/16);

const double s0 = 1/(2*sqrt(2));
const double s1 = 1/(4*cos(1*M_PI/16));
const double s2 = 1/(4*cos(2*M_PI/16));
const double s3 = 1/(4*cos(3*M_PI/16));
const double s4 = 1/(4*cos(4*M_PI/16));
const double s5 = 1/(4*cos(5*M_PI/16));
const double s6 = 1/(4*cos(6*M_PI/16));
const double s7 = 1/(4*cos(7*M_PI/16));

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
            ty4 = (-y4) - y5;
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
            auto a5a = (y4+y6) * a5;
            ty4 = (-(y4 * (a2))) - a5a;
            ty5 = y5 * a3;
            ty6 = (y6 * a4) - a5a;
            ty7 = y7;

            y0 = ty0;
            y1 = ty1;
            y2 = ty2 + ty3;
            y3 = ty2 - ty3;
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

            set(i, 0) += ty0 * s0;
            set(i, 1) += ty1 * s4;
            set(i, 2) += ty2 * s2;
            set(i, 3) += ty3 * s6;
            set(i, 4) += ty4 * s5;
            set(i, 5) += ty5 * s1;
            set(i, 6) += ty6 * s7;
            set(i, 7) += ty7 * s3;
        }
        for(uint i = 0; i < 8; i++) {
            double y0,y1,y2,y3,y4,y5,y6,y7;
            double ty0, ty1, ty2, ty3, ty4, ty5, ty6, ty7;
            y0 = get(0,i) + get(7,i);
            y1 = get(1,i) + get(6,i);
            y2 = get(2,i) + get(5,i);
            y3 = get(3,i) + get(4,i);
            y4 = get(3,i) - get(4,i);
            y5 = get(2,i) - get(5,i);
            y6 = get(1,i) - get(6,i);
            y7 = get(0,i) - get(7,i);

            ty0 = y0 + y3;
            ty1 = y1 + y2;
            ty2 = y1 - y2;
            ty3 = y0 - y3;
            ty4 = (-y4) - y5;
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
            auto a5a = (y4+y6) * a5;
            ty4 = (-(y4 * (a2))) - a5a;
            ty5 = y5 * a3;
            ty6 = (y6 * a4) - a5a;
            ty7 = y7;

            y0 = ty0;
            y1 = ty1;
            y2 = ty2 + ty3;
            y3 = ty2 - ty3;
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

            set(0,i) += ty0 * s0;
            set(1,i) += ty1 * s4;
            set(2,i) += ty2 * s2;
            set(3,i) += ty3 * s6;
            set(4,i) += ty4 * s5;
            set(5,i) += ty5 * s1;
            set(6,i) += ty6 * s7;
            set(7,i) += ty7 * s3;
        }
    }
};

#endif //MEDIENINFO_ARAICOSINTRANSFORM_H

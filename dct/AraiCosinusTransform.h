#ifndef MEDIENINFO_ARAICOSINTRANSFORM_H
#define MEDIENINFO_ARAICOSINTRANSFORM_H

#include "AbstractCosinusTransform.h"

template<typename T>
class DirectCosinusTransform : AbstractCosinusTransform<T> {


    void transformChannel(const std::vector<T>& channel, std::vector<T>& output, int xsize, int ysize) {

    }

};

#endif //MEDIENINFO_ARAICOSINTRANSFORM_H

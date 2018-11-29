#ifndef MEDIENINFO_ARAICOSINTRANSFORM_H
#define MEDIENINFO_ARAICOSINTRANSFORM_H

#include "AbstractCosinusTransform.h"

template<typename T>
class DirectCosinusTransform : AbstractCosinusTransform<T> {

    void transformChannel(const ColorChannel<T> &channel, std::vector<T> &output) override {

    }

};

#endif //MEDIENINFO_ARAICOSINTRANSFORM_H

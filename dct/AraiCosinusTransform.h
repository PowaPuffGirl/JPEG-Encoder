#ifndef MEDIENINFO_ARAICOSINTRANSFORM_H
#define MEDIENINFO_ARAICOSINTRANSFORM_H

#include "AbstractCosinusTransform.h"

template<typename T>
class DirectCosinusTransform : AbstractCosinusTransform<T, 8> {
    void transformBlock(std::function<T &(uint, uint)> get, std::function<T &(uint, uint)> set) override {

    }

};

#endif //MEDIENINFO_ARAICOSINTRANSFORM_H

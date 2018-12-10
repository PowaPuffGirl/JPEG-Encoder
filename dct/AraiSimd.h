//
// Created by proj on 12/10/18.
//

#ifndef MEDIENINFO_ARAISIMD_H
#define MEDIENINFO_ARAISIMD_H

#include "AbstractCosinusTransform.h"


template<typename T, unsigned int blocksize = 8>
class AraiSimd {
public:
    void transformBlock(const std::function<const T&(uint, uint)>& get, const std::function<T&(uint, uint)>& set) const {

    }
};


#endif //MEDIENINFO_ARAISIMD_H

#ifndef MEDIENINFO_DISCRETECOSINUSTRANSFORM_H
#define MEDIENINFO_DISCRETECOSINUSTRANSFORM_H

#include <vector>
#include "../Image.h"

template<typename T>
class AbstractCosinusTransform {
public:
    AbstractCosinusTransform() = default;
    virtual void transformChannel(const ColorChannel<T>& channel, std::vector<T>& output) = 0;
};


#endif //MEDIENINFO_DISCRETECOSINUSTRANSFORM_H

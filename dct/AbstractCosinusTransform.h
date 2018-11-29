#ifndef MEDIENINFO_DISCRETECOSINUSTRANSFORM_H
#define MEDIENINFO_DISCRETECOSINUSTRANSFORM_H

#include <vector>
#include <functional>
#include "../Image.h"

template<typename T, unsigned int blocksize = 8>
class AbstractCosinusTransform {
public:
    using uint = unsigned int;

    AbstractCosinusTransform() = default;
    virtual void transformBlock(std::function<T&(uint, uint)> get, std::function<T&(uint, uint)> set) = 0;
};


#endif //MEDIENINFO_DISCRETECOSINUSTRANSFORM_H

#ifndef MEDIENINFO_DISCRETECOSINUSTRANSFORM_H
#define MEDIENINFO_DISCRETECOSINUSTRANSFORM_H

#include <vector>

template<typename T>
class AbstractCosinusTransform {
public:
    AbstractCosinusTransform() = default;
    virtual void transformChannel(const std::vector<T>& channel, std::vector<T>& output) = 0;
};


#endif //MEDIENINFO_DISCRETECOSINUSTRANSFORM_H

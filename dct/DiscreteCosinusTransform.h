//
// Created by proj on 11/28/18.
//

#ifndef MEDIENINFO_DISCRETECOSINUSTRANSFORM_H
#define MEDIENINFO_DISCRETECOSINUSTRANSFORM_H

#include <vector>

template<typename T>
class DiscreteCosinusTransform {
public:
    DiscreteCosinusTransform() = default;
    void transformChannel(const std::vector<T>& channel, std::vector<T>& output) = 0;
};


#endif //MEDIENINFO_DISCRETECOSINUSTRANSFORM_H

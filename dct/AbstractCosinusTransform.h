#ifndef MEDIENINFO_DISCRETECOSINUSTRANSFORM_H
#define MEDIENINFO_DISCRETECOSINUSTRANSFORM_H

#include <vector>
#include <functional>
#include "../Image.h"

using uint = unsigned int;

template<typename T>
class FixedPointConverter {
public:

    constexpr static T convert(double input);
};

template<>
constexpr short FixedPointConverter<short>::convert(double input) {
    return static_cast<short>(input * std::numeric_limits<short>::max());
}

template<>
constexpr int32_t FixedPointConverter<int32_t>::convert(double input) {
    return static_cast<short>(input * std::numeric_limits<int32_t>::max());
}

template<>
constexpr double FixedPointConverter<double>::convert(double input) { return input; }
template<>
constexpr float FixedPointConverter<float>::convert(double input) { return static_cast<float>(input); }


#endif //MEDIENINFO_DISCRETECOSINUSTRANSFORM_H

#ifndef MEDIENINFO_QUNATISATIONTABLES_H
#define MEDIENINFO_QUNATISATIONTABLES_H

#include <array>

using QuantisationTable = std::array<int, 64>;

const static QuantisationTable luminaceOnePlus5 = {
        2,  1,  1,  2,  2,  4,  5,  6,
        1,  1,  1,  2,  3,  6,  6,  6,
        1,  1,  2,  2,  4,  6,  7,  6,
        1,  2,  2,  3,  5,  9,  8,  6,
        2,  2,  4,  6,  7, 11, 10,  8,
        2,  4,  6,  6,  8, 10, 11,  9,
        5,  6,  8,  9, 10, 12, 12, 10,
        7,  9, 10, 10, 11, 10, 10, 10
};

const static QuantisationTable chrominaceOnePlus5 = {
        2,  2,  2,  5, 10, 10, 10, 10,
        2,  2,  3,  7, 10, 10, 10, 10,
        2,  3,  6, 10, 10, 10, 10, 10,
        5,  7, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10
};

const static QuantisationTable luminaceOnePlus51 = {
        1,  1,  1,  1, 1, 1, 1, 1,
        1,  1,  1,  1, 1, 1, 1, 1,
        1,  1,  1, 1, 1, 1, 1, 1,
        1,  1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1
};

const static QuantisationTable chrominaceOnePlus51 = {
        1,  1,  1,  1, 1, 1, 1, 1,
        1,  1,  1,  1, 1, 1, 1, 1,
        1,  1,  1, 1, 1, 1, 1, 1,
        1,  1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1
};

#endif //MEDIENINFO_QUNATISATIONTABLES_H

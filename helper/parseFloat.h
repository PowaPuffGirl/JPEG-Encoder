#ifndef MEDIENINFO_PARSEFLOAT_H
#define MEDIENINFO_PARSEFLOAT_H

#define PARSEFLOAT_IMPL parseFloatSimple
#define parseFloat(str, strlen, result) PARSEFLOAT_IMPL(str, strlen, result)

#endif //MEDIENINFO_PARSEFLOAT_H

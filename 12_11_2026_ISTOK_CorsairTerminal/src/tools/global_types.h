#ifndef GLOBAL_TYPES_H
#define GLOBAL_TYPES_H

#include "stdint.h"

typedef struct _t_point
{
    uint16_t x;
    uint16_t y;
} t_point;

typedef enum
{
    en_old,
    en_notgoodnotterrible,
    en_new
} t_detector_relevance;

#endif // GLOBAL_TYPES_H

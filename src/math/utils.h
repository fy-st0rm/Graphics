#ifndef __MATH_UTILS_H__
#define __MATH_UTILS_H__

#include "core/defines.h"

#include <stdlib.h>
#include <math.h>

#define PE_PI 3.14159
#define to_radians(x) x * PE_PI / 180
#define to_degrees(x) x * 180 / PE_PI
#define rand_init(seed) srand(seed)
#define rand_range(l, u) rand() % (u - l + 1) + l

static b32 f32_eq(f32 a, f32 b) {
	return fabs(a - b) < 0.01f;
}

#endif // __MATH_UTILS_H__

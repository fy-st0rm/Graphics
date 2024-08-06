#ifndef __MATH_UTILS_H__
#define __MATH_UTILS_H__

#include "core/defines.h"

#include <stdlib.h>
#include <math.h>

#define PI 3.14159
#define to_radians(x) ((x) * PE_PI / 180)
#define to_degrees(x) ((x) * 180 / PE_PI)
#define rand_init(seed) srand(seed)
#define rand_range(l, u) rand() % (u - l + 1) + l

static b32 f32_eq(f32 a, f32 b) {
	return fabs(a - b) < 0.01f;
}

static v2 pixel_to_gl_coords(v2 pos, u32 WIN_WIDTH, u32 WIN_HEIGHT) {
	return (v2) {
		(2 * pos.x) / WIN_WIDTH - 1,
		1 - (2 * pos.y) / WIN_HEIGHT
	};
}

static v2 gl_to_pixel_coords(v2 pos, u32 WIN_WIDTH, u32 WIN_HEIGHT) {
	return (v2) {
		(pos.x + 1) * WIN_WIDTH / 2,
		(1 - pos.y) * WIN_HEIGHT / 2
	};
}


#endif // __MATH_UTILS_H__

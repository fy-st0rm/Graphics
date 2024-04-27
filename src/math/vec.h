#ifndef __VEC_H__
#define __VEC_H__

#include "core/defines.h"

/*
 * @brief Vector definitions
 */

typedef struct {
	f32 x, y;
} v2;

typedef struct {
	f32 x, y, z;
} v3;

typedef struct {
	union { f32 x, r; };
	union { f32 y, g; };
	union { f32 z, b; };
	union { f32 w, a; };
} v4;

/*
 * @brief Vector print functions
 */

void print_v2(v2 v);
void print_v3(v3 v);
void print_v4(v4 v);

/*
 * @brief Vector compares
 */

b8 v2_eq(v2 a, v2 b);
b8 v3_eq(v3 a, v3 b);
b8 v4_eq(v4 a, v4 b);

/*
 * @brief Vector arithematics
 */

v2 v2_add(v2 a, v2 b);
v3 v3_add(v3 a, v3 b);
v4 v4_add(v4 a, v4 b);

v2 v2_sub(v2 a, v2 b);
v3 v3_sub(v3 a, v3 b);
v4 v4_sub(v4 a, v4 b);

v2 v2_mul(v2 a, v2 b);
v3 v3_mul(v3 a, v3 b);
v4 v4_mul(v4 a, v4 b);

v2 v2_mul_scalar(v2 v, f32 scalar);
v3 v3_mul_scalar(v3 v, f32 scalar);
v4 v4_mul_scalar(v4 v, f32 scalar);

v2 v2_div(v2 a, v2 b);
v3 v3_div(v3 a, v3 b);
v4 v4_div(v4 a, v4 b);

f32 v2_mag(v2 v);
f32 v3_mag(v3 v);
f32 v4_mag(v4 v);

v2 v2_dir(v2 v);
v3 v3_dir(v3 v);

v2 v2_normalize(v2 v);
v3 v3_normalize(v3 v);
v4 v4_normalize(v4 v);

v2 v2_cross(v2 a, v2 b);
v3 v3_cross(v3 a, v3 b);
v4 v4_cross(v4 a, v4 b);

#endif // __VEC_H__

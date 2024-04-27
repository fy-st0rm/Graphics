#ifndef __MAT_H__
#define __MAT_H__

#include "core/defines.h"
#include "vec.h"

typedef struct {
	f32 m[4][4];
} m4;

void print_m4(m4 m);
void m4_clear(m4* m);
m4 m4_mul(m4 m1, m4 m2);
v3 m4_mul_v3(m4 m, v3 v);
m4 m4_identity();
m4 m4_inverse(m4 in);
m4 m4_translate(m4 m, v3 v);
m4 m4_transpose(m4 m);
m4 ortho_projection(f32 left, f32 right, f32 top, f32 bottom, f32 near, f32 far);
m4 persp_projection(f32 aspect_ratio, f32 fov, f32 near, f32 far);
m4 rotate_x(f32 theta);
m4 rotate_y(f32 theta);
m4 rotate_z(f32 theta);

#endif // __MAT_H__

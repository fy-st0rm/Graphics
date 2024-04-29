#include "mat.h"
#include "utils.h"

#include <string.h>

void print_m4(m4 m) {
	for (i32 i = 0; i < 4; i++) {
		for (i32 j = 0; j < 4; j++) {
			printf("%f\t", m.m[i][j]);
		}
		printf("\n");
	}
}

void m4_clear(m4* m) {
	memset(m, 0, sizeof(m4));
}

m4 m4_mul(m4 m1, m4 m2) {
	m4 out;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			out.m[i][j] = 0;
			for (int k = 0; k < 4; k++) {
				out.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return out;
}

v3 m4_mul_v3(m4 m, v3 v) {
	v3 out;
	out.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0];
	out.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1];
	out.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2];
	f32 w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];
	if (w) {
		out.x /= w;
		out.y /= w;
		out.z /= w;
	}
	return out;
}

m4 m4_identity() {
	return (m4) {
		.m = {
			{ 1.0f, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f }
		}
	};
}

m4 m4_zero() {
	return (m4) {
		.m = {
			{ 0.0f, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 0.0f }
		}
	};
}

m4 m4_inverse(m4 in) {
	m4 out;
	m4_clear(&out);
	out.m[0][0] = in.m[0][0]; 
	out.m[0][1] = in.m[1][0]; 
	out.m[0][2] = in.m[2][0]; 
	out.m[0][3] = 0.0f;
	out.m[1][0] = in.m[0][1]; 
	out.m[1][1] = in.m[1][1];
	out.m[1][2] = in.m[2][1];
	out.m[1][3] = 0.0f;
	out.m[2][0] = in.m[0][2];
	out.m[2][1] = in.m[1][2];
	out.m[2][2] = in.m[2][2];
	out.m[2][3] = 0.0f;
	out.m[3][0] = -(in.m[3][0] * out.m[0][0] + in.m[3][1] * out.m[1][0] + in.m[3][2] * out.m[2][0]);
	out.m[3][1] = -(in.m[3][0] * out.m[0][1] + in.m[3][1] * out.m[1][1] + in.m[3][2] * out.m[2][1]);
	out.m[3][2] = -(in.m[3][0] * out.m[0][2] + in.m[3][1] * out.m[1][2] + in.m[3][2] * out.m[2][2]);
	out.m[3][3] = 1.0f;
	return out;
}

m4 m4_translate(m4 m, v3 v) {
	return (m4) {
		.m = {
			{      1.0f, m.m[0][1], m.m[0][2],  v.x },
			{ m.m[1][0],      1.0f, m.m[1][2],  v.y },
			{ m.m[2][0], m.m[2][1],      1.0f,  v.z },
			{ m.m[3][0], m.m[3][1], m.m[3][2], 1.0f },
		}
	};
}

m4 m4_transpose(m4 m) {
	return (m4) {
		.m = {
			{ m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0] },
			{ m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1] },
			{ m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2] },
			{ m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3] }
		}
	};
}

m4 ortho_projection(f32 left, f32 right, f32 top, f32 bottom, f32 near, f32 far) {
	f32 x_range = right - left;
	f32 y_range = top - bottom;
	f32 z_range = far - near;

	return (m4) {
		.m = {
			{               2 / x_range,                         0,                       0, 0 },
			{                         0,               2 / y_range,                       0, 0 },
			{                         0,                         0,            -2 / z_range, 0 },
			{ -(right + left) / x_range, -(top + bottom) / y_range, -(far + near) / z_range, 1 }
		}
	};
}

m4 persp_projection(f32 aspect_ratio, f32 fov, f32 near, f32 far) {
	f32 t = tanf(to_radians(fov / 2));
	f32 z_range = near - far;
	f32 A = (-far - near) / z_range;
	f32 B = (2 * far * near) / z_range;

	return (m4) {
		.m = {
			{ 1 / (aspect_ratio * t),     0, 0, 0 },
			{                      0, 1 / t, 0, 0 },
			{                      0,     0, A, B },
			{                      0,     0, 1, 0 }
		}
	};
}

m4 rotate_x(f32 theta) {
	return (m4) {
		.m = {
			{ 1.0f,         0.0f,        0.0f, 0.0f },
			{ 0.0f,  cosf(theta), sinf(theta), 0.0f },
			{ 0.0f, -sinf(theta), cosf(theta), 0.0f },
			{ 0.0f,         0.0f,        0.0f, 1.0f }
		}
	};
}


m4 rotate_y(f32 theta) {
	return (m4) {
		.m = {
			{  cosf(theta),  0.0f, sinf(theta), 0.0f },
			{         0.0f,  1.0f,        0.0f, 0.0f },
			{ -sinf(theta),  0.0f, cosf(theta), 0.0f },
			{         0.0f,  0.0f,        0.0f, 1.0f }
		}
	};
}

m4 rotate_z(f32 theta) {
	return (m4) {
		.m = {
			{  cosf(theta), sinf(theta), 0.0f, 0.0f },
			{ -sinf(theta), cosf(theta), 0.0f, 0.0f },
			{         0.0f,        0.0f, 1.0f, 0.0f },
			{         0.0f,        0.0f, 0.0f, 1.0f }
		}
	};
}


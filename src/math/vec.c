#include "vec.h"
#include "utils.h"

/*
 * @brief Vector print functions
 */

void print_v2(v2 v) {
	printf("(%f, %f)\n", v.x, v.y);
}

void print_v3(v3 v) {
	printf("(%f, %f, %f)\n", v.x, v.y, v.z);
}

void print_v4(v4 v) {
	printf("(%f, %f, %f, %f)\n", v.x, v.y, v.z, v.w);
}

/*
 * @brief Vector compares
 */

b32 v2_eq(v2 a, v2 b) {
	return f32_eq(a.x, b.x) &&
         f32_eq(a.y, b.y);
}

b32 v3_eq(v3 a, v3 b) {
	return f32_eq(a.x, b.x) &&
         f32_eq(a.y, b.y) &&
         f32_eq(a.z, b.z);
}

b32 v4_eq(v4 a, v4 b) {
	return f32_eq(a.x, b.x) &&
         f32_eq(a.y, b.y) &&
         f32_eq(a.z, b.z) &&
         f32_eq(a.w, b.w);
}

/*
 * @brief Vector arithematics
 */

v2 v2_add(v2 a, v2 b) {
	return (v2) {
		.x = a.x + b.x,
		.y = a.y + b.y
	};
}

v3 v3_add(v3 a, v3 b) {
	return (v3) {
		.x = a.x + b.x,
		.y = a.y + b.y,
		.z = a.z + b.z
	};
}

v4 v4_add(v4 a, v4 b) {
	return (v4) {
		.x = a.x + b.x,
		.y = a.y + b.y,
		.z = a.z + b.z,
		.w = a.w + b.w
	};
}

v2 v2_sub(v2 a, v2 b) {
	return (v2) {
		.x = a.x - b.x,
		.y = a.y - b.y
	};
}

v3 v3_sub(v3 a, v3 b) {
	return (v3) {
		.x = a.x - b.x,
		.y = a.y - b.y,
		.z = a.z - b.z
	};
}

v4 v4_sub(v4 a, v4 b) {
	return (v4) {
		.x = a.x - b.x,
		.y = a.y - b.y,
		.z = a.z - b.z,
		.w = a.w - b.w
	};
}

v2 v2_mul(v2 a, v2 b) {
	return (v2) {
		.x = a.x * b.x,
		.y = a.y * b.y
	};
}

v3 v3_mul(v3 a, v3 b) {
	return (v3) {
		.x = a.x * b.x,
		.y = a.y * b.y,
		.z = a.z * b.z
	};
}

v4 v4_mul(v4 a, v4 b) {
	return (v4) {
		.x = a.x * b.x,
		.y = a.y * b.y,
		.z = a.z * b.z,
		.w = a.w * b.w
	};
}

v2 v2_mul_scalar(v2 v, f32 scalar) {
	return (v2) {
		.x = v.x * scalar,
		.y = v.y * scalar
	};
}

v3 v3_mul_scalar(v3 v, f32 scalar) {
	return (v3) {
		.x = v.x * scalar,
		.y = v.y * scalar,
		.z = v.z * scalar
	};
}

v4 v4_mul_scalar(v4 v, f32 scalar) {
	return (v4) {
		.x = v.x * scalar,
		.y = v.y * scalar,
		.z = v.z * scalar,
		.w = v.w * scalar
	};
}

v2 v2_div(v2 a, v2 b) {
	return (v2) {
		.x = a.x / b.x,
		.y = a.y / b.y
	};
}

v3 v3_div(v3 a, v3 b) {
	return (v3) {
		.x = a.x / b.x,
		.y = a.y / b.y,
		.z = a.z / b.z
	};
}

v4 v4_div(v4 a, v4 b) {
	return (v4) {
		.x = a.x / b.x,
		.y = a.y / b.y,
		.z = a.z / b.z,
		.w = a.w / b.w
	};
}

f32 v2_mag(v2 v) {
	return sqrt(v.x * v.x + v.y * v.y);
}

f32 v3_mag(v3 v) {
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

f32 v4_mag(v4 v) {
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

v2 v2_dir(v2 v) {
	v2 d = { 0, 0 };
	if (v.x < 0) d.x = -1;
	if (v.x > 0) d.x = 1;
	if (v.x == 0) d.x = 0;

	if (v.y < 0) d.y = -1;
	if (v.y > 0) d.y = 1;
	if (v.y == 0) d.y = 0;
	return d;
}

v3 v3_dir(v3 v) {
	v3 d = { 0, 0 };
	if (v.x < 0) d.x = -1;
	if (v.x > 0) d.x = 1;
	if (v.x == 0) d.x = 0;

	if (v.y < 0) d.y = -1;
	if (v.y > 0) d.y = 1;
	if (v.y == 0) d.y = 0;

	if (v.z < 0) d.z = -1;
	if (v.z > 0) d.z = 1;
	if (v.z == 0) d.z = 0;
	return d;
}

v2 v2_normalize(v2 v) {
	f32 r = v2_mag(v);
	return (v2) {
		.x = v.x / r,
		.y = v.y / r
	};
}

v3 v3_normalize(v3 v) {
	f32 r = v3_mag(v);
	return (v3) {
		.x = v.x / r,
		.y = v.y / r,
		.z = v.z / r
	};
}
v4 v4_normalize(v4 v) {
	f32 r = v4_mag(v);
	return (v4) {
		.x = v.x / r,
		.y = v.y / r,
		.z = v.z / r,
		.w = v.w / r
	};
}

v2 v2_cross(v2 a, v2 b) {
	assert(false, "v2_cross is not implemented yet.\n");
}

v3 v3_cross(v3 a, v3 b) {
	return (v3) {
		.x = a.y * b.z - a.z * b.y,
		.y = - (a.x * b.z - a.z * b.x),
		.z = a.x * b.y - a.y * b.x
	};
}

v4 v4_cross(v4 a, v4 b) {
	assert(false, "v3_cross is not implemented yet.\n");
}

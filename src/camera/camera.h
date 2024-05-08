#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "math/vec.h"
#include "math/mat.h"

// Orthographic camera

typedef struct {
	f32 left, right, top, bottom, near, far;
} OCamera_Boundary;

typedef struct {
	v2 pos;
	f32 zoom;
	m4 mvp;
	OCamera_Boundary org_b;
	OCamera_Boundary boundary;
} OCamera;

OCamera ocamera_new(v2 pos, f32 zoom, OCamera_Boundary boundary);
void ocamera_change_zoom(OCamera* cam, f32 dz);
void ocamera_change_pos(OCamera* cam, v2 dp);
m4 ocamera_calc_mvp(OCamera* cam);


// Perspective camera

typedef struct {
	f32 aspect_ratio, fov, near, far;
} PCamera_Info;

typedef struct {
	v3 dir, up, right;
	v3 pos;
	m4 look_at, mvp;
	PCamera_Info info;
} PCamera;

PCamera pcamera_new(v3 pos, v3 dir, PCamera_Info info);
void pcamera_change_pos(PCamera* cam, v3 dp);
m4 pcamera_calc_mvp(PCamera* cam);

#endif // __CAMERA_H__

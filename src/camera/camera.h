#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "math/vec.h"
#include "math/mat.h"

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
m4 ocamera_calc_mvp(OCamera* cam);

#endif // __CAMERA_H__

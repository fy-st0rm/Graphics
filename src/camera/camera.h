#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "window/window.h"
#include "event/event.h"
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
	f32 pitch, yaw;

	// Mouse
	v2 mp;
	f32 sensitivity;
	b32 first;
	b32 mouse_enable;

	PCamera_Info info;
} PCamera;

PCamera pcamera_new(v3 pos, v3 dir, f32 sensitivity, PCamera_Info info);
void pcamera_change_pos(PCamera* cam, v3 dp);
void pcamera_handle_mouse(PCamera* cam, Window window);
m4 pcamera_calc_mvp(PCamera* cam);

#endif // __CAMERA_H__

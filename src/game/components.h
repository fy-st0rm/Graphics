#ifndef __COMPONENTS_H__
#define __COMPONENTS_H__

#include "graphics/texture.h"

typedef struct {
	v3 pos;
	v2 size;
} TransformComponent;

typedef struct {
	v4 color;
	Texture texture;
	Rect tex_coord;
} RenderComponent;

typedef struct {
	v2 pos;
	f32 intensity;
	f32 radius;
	f32 fov;
	f32 dir;
	v4 color;
} LightComponent;

#endif // __COMPONENTS_H__

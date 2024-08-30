#ifndef __COMPONENTS_H__
#define __COMPONENTS_H__

#include "window/window.h"
#include "core/dyn_array.h"
#include "graphics/texture.h"
#include "math/rect.h"
#include "math/vec.h"

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

typedef struct {
	i32 id;
	Dyn_Array(Rect) frames;
	f32 duration;
} AnimationEntry;

// TODO: Transfer this entries from dynamic array to a hash map
typedef struct {
	Dyn_Array(AnimationEntry) entries;
	i32 curr_state;
	f64 start_time;
} AnimationComponent;

AnimationComponent make_animation_component(void* entries, i32 starting_state);
void ac_switch_frame(AnimationComponent* ac, i32 id);
Rect ac_get_frame(AnimationComponent* ac);

#endif // __COMPONENTS_H__

#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "math/vec.h"
#include "math/mat.h"
#include "graphics/imr.h"
#include "graphics/fbo.h"
#include "ecs/ecs.h"
#include "camera/camera.h"

#include "components.h"

typedef struct {
	IMR imr;
	ECS* ecs;
	OCamera final_cam;
	v2 surf_size, win_size;

	FBO light_fbo, color_fbo, mix_fbo;
} Renderer;

RESULT(Renderer, Renderer);

Result_Renderer renderer_new(ECS* ecs, v2 surf_size, v2 win_size);
void renderer_delete(Renderer* ren);
void renderer_update(Renderer* ren, OCamera* camera, v4 color);

#endif // __RENDERER_H__

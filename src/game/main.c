#include "window/window.h"
#include "graphics/imr.h"
#include "math/vec.h"
#include "ecs/ecs.h"

#include "renderer.h"

#define MAX_ENTITY_CNT 100

const v2 WIN_SIZE = { 800, 600 };
const v2 SURF_SIZE = { 133, 100 };

int main(int argc, char** argv) {
	Window window = unwrap(window_new("Game", WIN_SIZE.x, WIN_SIZE.y));
	ECS* ecs = ecs_new(MAX_ENTITY_CNT);

	Renderer ren = unwrap(renderer_new(ecs, SURF_SIZE, WIN_SIZE));

	Entity ent = entity_new(ecs);
	entity_add_component(ecs, ent, TransformComponent, (v3){0,0,0}, (v2){0.1, 0.1});
	entity_add_component(ecs, ent, RenderComponent, (v4) { 0, 1, 0, 1 });

	while (!window.should_close) {
		renderer_update(&ren, (v4) { 0, 0, 0, 1 });
		window_update(&window);
	}
	
	window_delete(window);
	return 0;
}

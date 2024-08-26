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

	OCamera camera = ocamera_new(
		(v2) { 0, 0 },
		1.0f,
		(OCamera_Boundary) { 0, SURF_SIZE.x, SURF_SIZE.y, 0, -1, 1000 }
	);

	Renderer ren = unwrap(renderer_new(ecs, SURF_SIZE, WIN_SIZE));

	// Entity
	v2 size = { 20, 20 };
	Entity ent = entity_new(ecs);
	entity_add_component(
		ecs, ent, TransformComponent,
		(v3) {
			SURF_SIZE.x / 2 - size.x / 2,
			SURF_SIZE.y / 2 - size.y / 2,
			0
		},
		size
	);
	entity_add_component(ecs, ent, RenderComponent, (v4) { 0, 1, 0, 1 });

	// Light
	Entity light_1 = entity_new(ecs);
	entity_add_component(
		ecs, light_1, LightComponent, 
		(v2) {
			SURF_SIZE.x / 2 - 40, SURF_SIZE.y / 2 + 20
		},
		1.0f,
		0.9f,
		PI / 2,
		to_radians(45),
		(v4){ 1, 0.1, 0.1, 1 }
	);

	Entity light_2 = entity_new(ecs);
	entity_add_component(
		ecs, light_2, LightComponent, 
		(v2) {
			SURF_SIZE.x / 2 + 40, SURF_SIZE.y / 2 + 20
		},
		1.0f,
		0.9f,
		PI / 2,
		to_radians(135),
		(v4){ 0, 0.5, 0.8, 1 }
	);


	float speed = 1.0f;
	while (!window.should_close) {

		// Event
		{
			TransformComponent* tc = entity_get_component(ecs, ent, TransformComponent);
			Event event;
			while(event_poll(window, &event)) {
				if (event.type == KEYDOWN) {
					switch (event.e.key) {
						case GLFW_KEY_EQUAL:
							ocamera_change_zoom(&camera, 0.1);
							break;
						case GLFW_KEY_MINUS:
							ocamera_change_zoom(&camera, -0.1);
							break;

						case GLFW_KEY_W:
							tc->pos = v3_add(tc->pos, (v3) { 0, speed, 0 });
							//ocamera_change_pos(&camera, (v2) { 0, speed });
							break;
						case GLFW_KEY_A:
							tc->pos = v3_add(tc->pos, (v3) { -speed, 0, 0 });
							//ocamera_change_pos(&camera, (v2) { -speed, 0 });
							break;
						case GLFW_KEY_S:
							tc->pos = v3_add(tc->pos, (v3) { 0, -speed, 0 });
							// ocamera_change_pos(&camera, (v2) { 0, -speed });
							break;
						case GLFW_KEY_D:
							tc->pos = v3_add(tc->pos, (v3) { speed, 0, 0 });
							//ocamera_change_pos(&camera, (v2) { speed, 0 });
							break;
					}
				}
			}
		}

		renderer_update(&ren, &camera, (v4) { 0, 0, 0, 1 });
		window_update(&window);
	}
	
	window_delete(window);
	return 0;
}

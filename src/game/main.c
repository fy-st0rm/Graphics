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

	Entity ent = entity_new(ecs);
	entity_add_component(ecs, ent, TransformComponent, (v3){10,10,0}, (v2){10, 10});
	entity_add_component(ecs, ent, RenderComponent, (v4) { 0, 1, 0, 1 });

	while (!window.should_close) {

		// Event
		{
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
							ocamera_change_pos(&camera, (v2) { 0, 0.1 });
							break;
						case GLFW_KEY_A:
							ocamera_change_pos(&camera, (v2) { -0.1, 0 });
							break;
						case GLFW_KEY_S:
							ocamera_change_pos(&camera, (v2) { 0, -0.1 });
							break;
						case GLFW_KEY_D:
							ocamera_change_pos(&camera, (v2) { 0.1, 0 });
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

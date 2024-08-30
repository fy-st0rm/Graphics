#include "window/window.h"
#include "graphics/imr.h"
#include "math/vec.h"
#include "ecs/ecs.h"

#include "components.h"
#include "renderer.h"
#include "config.h"

typedef enum {
	IDLE
} AnimationID;

Entity player_init(ECS* ecs) {
	Texture player_sprite = unwrap(texture_from_file(PLAYER_SPRITE, true));
	texture_bind(player_sprite);

	Entity player = entity_new(ecs);
	entity_add_component(
		ecs, player, TransformComponent, {
			(v3) {
				SURF_SIZE.x / 2 - PLAYER_SIZE.x / 2 - 30,
				SURF_SIZE.y / 2 - PLAYER_SIZE.y / 2 - 10,
				0
			},
			PLAYER_SIZE
		}
	);
	entity_add_component(
		ecs, player, RenderComponent, {
			(v4) { 1, 1, 1, 1 },
			player_sprite,
			(Rect) { 0, 0, 1.0f / PLAYER_SPRITE_X_CNT, 1.0f / PLAYER_SPRITE_Y_CNT },
		}
	);

	Dyn_Array(Rect) idle_frames = NULL;
	for (i32 i = 0; i < 14; i++) {
		dyn_array_append(
			idle_frames,
			(Rect) { (f32)i / PLAYER_SPRITE_X_CNT, 0, 1.0f / PLAYER_SPRITE_X_CNT, 1.0f / PLAYER_SPRITE_Y_CNT }
		);
	}

	AnimationEntry idle_entry = {
		.id = IDLE,
		.frames = (void*) idle_frames,
		.duration= 100.0f * 14
	};

	Dyn_Array(AnimationEntry) entries = NULL;
	dyn_array_append(entries, idle_entry);

	entity_add_component(
		ecs, player,
		AnimationComponent,
		make_animation_component((void*) entries, IDLE)
	);

	return player;
}

// Main

int main(int argc, char** argv) {
	Window window = unwrap(window_new("Game", WIN_SIZE.x, WIN_SIZE.y));
	ECS* ecs = ecs_new(MAX_ENTITY_CNT);

	OCamera camera = ocamera_new(
		(v2) { 0, 0 },
		1.0f,
		(OCamera_Boundary) { 0, SURF_SIZE.x, SURF_SIZE.y, 0, -1, 1000 }
	);

	Renderer ren = unwrap(renderer_new(ecs, SURF_SIZE, WIN_SIZE));

	Entity player = player_init(ecs);

	float speed = 1.0f;
	while (!window.should_close) {

		// Event
		{
			TransformComponent* tc = entity_get_component(ecs, player, TransformComponent);
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

		renderer_update(&ren, &camera, (v4) { 0.5, 0.5, 0.5, 1 });
		window_update(&window);
	}
	
	ecs_delete(ecs);
	window_delete(window);
	return 0;
}

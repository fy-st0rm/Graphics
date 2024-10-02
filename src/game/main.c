#include "window/window.h"
#include "graphics/imr.h"
#include "math/vec.h"
#include "ecs/ecs.h"

#include "components.h"
#include "renderer.h"
#include "config.h"

typedef enum {
	IDLE, WALK
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
			PLAYER_SIZE,
			rotate_y(0)
		}
	);

	entity_add_component(
		ecs, player, MovementComponent, {
			M_NONE,
			M_RIGHT,
			1.5f
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
	for (i32 i = 0; i < 8; i++) {
		dyn_array_append(
			idle_frames,
			(Rect) {
				(f32)i / PLAYER_SPRITE_X_CNT,
				7.0f / PLAYER_SPRITE_Y_CNT,
				1.0f / PLAYER_SPRITE_X_CNT,
				1.0f / PLAYER_SPRITE_Y_CNT
			}
		);
	}

	Dyn_Array(Rect) walk_frames = NULL;
	for (i32 i = 0; i < 8; i++) {
		dyn_array_append(
			walk_frames,
			(Rect) {
				(f32)i / PLAYER_SPRITE_X_CNT,
				6.0f / PLAYER_SPRITE_Y_CNT,
				1.0f / PLAYER_SPRITE_X_CNT,
				1.0f / PLAYER_SPRITE_Y_CNT
			}
		);
	}

	AnimationEntry idle_entry = {
		.id = IDLE,
		.frames = (void*) idle_frames,
		.duration= 100.0f * 8
	};

	AnimationEntry walk_entry = {
		.id = WALK,
		.frames = (void*) walk_frames,
		.duration= 100.0f * 8
	};

	Dyn_Array(AnimationEntry) entries = NULL;
	dyn_array_append(entries, idle_entry);
	dyn_array_append(entries, walk_entry);

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

	float speed = 5.0f;
	while (!window.should_close) {

		// Event
		{
			TransformComponent* tc = entity_get_component(ecs, player, TransformComponent);
			MovementComponent* mc = entity_get_component(ecs, player, MovementComponent);
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

						case GLFW_KEY_A: {
							if (mc->look_dir == M_RIGHT) {
								tc->rot = rotate_y(PI);
							}
							mc->h_dir = M_LEFT;
							mc->look_dir = M_LEFT;
							break;
						}

						case GLFW_KEY_D: {
							if (mc->look_dir == M_LEFT) {
								tc->rot = rotate_y(2 * PI);
							}
							mc->h_dir = M_RIGHT;
							mc->look_dir = M_RIGHT;
							break;
						}
					}
				}
				else if (event.type == KEYUP) {
					switch (event.e.key) {
						case GLFW_KEY_A:
							mc->h_dir = M_NONE;
							break;

						case GLFW_KEY_D:
							mc->h_dir = M_NONE;
							break;
					}
				}
			}
		}

		// Movement Update
		{
			TransformComponent* tc = entity_get_component(ecs, player, TransformComponent);
			AnimationComponent* ac = entity_get_component(ecs, player, AnimationComponent);
			MovementComponent* mc = entity_get_component(ecs, player, MovementComponent);
			if (mc->h_dir == M_LEFT) {
				tc->pos = v3_add(tc->pos, (v3) {-mc->speed, 0, 0});
				ac_switch_frame(ac, WALK);
			} else if (mc->h_dir == M_RIGHT) {
				tc->pos = v3_add(tc->pos, (v3) {mc->speed, 0, 0});
				ac_switch_frame(ac, WALK);
			} else {
				ac_switch_frame(ac, IDLE);
			}
		}

		renderer_update(&ren, &camera, (v4) { 0.5, 0.5, 0.5, 1 });
		window_update(&window);
	}
	
	ecs_delete(ecs);
	window_delete(window);
	return 0;
}

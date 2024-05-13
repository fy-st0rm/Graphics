#include "window/window.h"
#include "graphics/imr.h"
#include "camera/camera.h"
#include "event/event.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define SPEED 0.05f

typedef enum {
	LEFT, RIGHT, UP, DOWN, FRONT, BACK, TOTAL
} Direction;

int main() {
	Window window = Result_Window_unwrap(
		window_new("3D", WIN_WIDTH, WIN_HEIGHT)
	);
	IMR imr = Result_IMR_unwrap(imr_new());

	PCamera cam = pcamera_new(
		(v3) { 0.05, -0.3, 2.4 },
		(v3) { 0, 0, -1 },
		0.1f,
		(PCamera_Info) {
			.aspect_ratio = (f32) WIN_WIDTH / WIN_HEIGHT,
			.fov = 45.0f,
			.near = 0.0f,
			.far = 1000.0f
		}
	);
	b32 movement[TOTAL];

	// Backgrounds
	Texture bg_1 = Result_Texture_unwrap(
		texture_from_file(
			"assets/oak_woods/background/background_layer_1.png",
			true
		)
	);
	texture_bind(bg_1);

	Texture bg_2 = Result_Texture_unwrap(
		texture_from_file(
			"assets/oak_woods/background/background_layer_2.png",
			true
		)
	);
	texture_bind(bg_2);

	Texture bg_3 = Result_Texture_unwrap(
		texture_from_file(
			"assets/oak_woods/background/background_layer_3.png",
			true
		)
	);
	texture_bind(bg_3);

	// Ground
	Texture ground = Result_Texture_unwrap(
		texture_from_file(
			"assets/oak_woods/background/ground.png",
			true
		)
	);
	texture_bind(ground);

	// Fence
	Texture fence = Result_Texture_unwrap(
		texture_from_file(
			"assets/oak_woods/decorations/fence_1.png",
			true
		)
	);
	texture_bind(fence);

	// Shop
	Texture shop = Result_Texture_unwrap(
		texture_from_file(
			"assets/oak_woods/decorations/shop.png",
			true
		)
	);
	texture_bind(shop);

	// Grass
	Texture grass_1 = Result_Texture_unwrap(
		texture_from_file(
			"assets/oak_woods/decorations/grass_1.png",
			true
		)
	);
	texture_bind(grass_1);

	Texture grass_2 = Result_Texture_unwrap(
		texture_from_file(
			"assets/oak_woods/decorations/grass_2.png",
			true
		)
	);
	texture_bind(grass_2);

	Texture grass_3 = Result_Texture_unwrap(
		texture_from_file(
			"assets/oak_woods/decorations/grass_3.png",
			true
		)
	);
	texture_bind(grass_3);

	// Lamp
	Texture lamp = Result_Texture_unwrap(
		texture_from_file(
			"assets/oak_woods/decorations/lamp.png",
			true
		)
	);
	texture_bind(lamp);

	// Rock
	Texture rock_3 = Result_Texture_unwrap(
		texture_from_file(
			"assets/oak_woods/decorations/rock_3.png",
			true
		)
	);
	texture_bind(rock_3);

	// Player
	Texture pl_tex = Result_Texture_unwrap(
		texture_from_file("assets/oak_woods/character/char_blue.png", true)
	);
	texture_bind(pl_tex);

	v3 pl_pos = { 0, 0, 0 };

	while (!window.should_close) {
		Event event;
		while(event_poll(window, &event)) {
			if (event.type == KEYDOWN) {
				switch (event.e.key) {
					case GLFW_KEY_ESCAPE:
						cam.mouse_enable = cam.mouse_enable ? false : true;
						break;

					case GLFW_KEY_W:
						movement[FRONT] = true;
						break;
					case GLFW_KEY_A:
						movement[LEFT] = true;
						break;
					case GLFW_KEY_S:
						movement[BACK] = true;
						break;
					case GLFW_KEY_D:
						movement[RIGHT] = true;
						break;

					case GLFW_KEY_SPACE:
						movement[UP] = true;
						break;
					case GLFW_KEY_LEFT_SHIFT:
						movement[DOWN] = true;
						break;
				}
			}
			else if (event.type == KEYUP) {
				switch (event.e.key) {
					case GLFW_KEY_W:
						movement[FRONT] = false;
						break;
					case GLFW_KEY_A:
						movement[LEFT] = false;
						break;
					case GLFW_KEY_S:
						movement[BACK] = false;
						break;
					case GLFW_KEY_D:
						movement[RIGHT] = false;
						break;

					case GLFW_KEY_SPACE:
						movement[UP] = false;
						break;
					case GLFW_KEY_LEFT_SHIFT:
						movement[DOWN] = false;
						break;
				}
			}
		}

		if (movement[FRONT]) {
			pcamera_change_pos(&cam, v3_mul_scalar(cam.dir, SPEED));
		}
		if (movement[BACK]) {
			pcamera_change_pos(&cam, v3_mul_scalar(cam.dir, -SPEED));
		}
		if (movement[LEFT]) {
			pcamera_change_pos(&cam, v3_mul_scalar(cam.right, -SPEED));
		}
		if (movement[RIGHT]) {
			pcamera_change_pos(&cam, v3_mul_scalar(cam.right, SPEED));
		}
		if (movement[UP]) {
			pcamera_change_pos(&cam, v3_mul_scalar(cam.up, SPEED));
		}
		if (movement[DOWN]) {
			pcamera_change_pos(&cam, v3_mul_scalar(cam.up, -SPEED));
		}

		pcamera_handle_mouse(&cam, window);
		m4 mvp = pcamera_calc_mvp(&cam);
		imr_update_mvp(&imr, mvp);

		imr_clear((v4) { 97.0/255, 83.0/255, 109.0/255, 1 });
		imr_begin(&imr);

		// Background
		v2 size = { 1.5, 1 };
		for (i32 i = -2; i < 2; i++) {
			v3 pos = { i * size.x - size.x / 2, 0 - size.y / 2, -1};
			imr_push_quad_tex(
				&imr,
				pos,
				size,
				(Rect) { 0, 0, 1, 1 },
				bg_1.id,
				rotate_z(0),
				(v4) { 1, 1, 1, 1 }
			);
		}

		for (i32 i = -2; i < 2; i++) {
			v3 pos = { i * size.x - size.x / 2, 0 - size.y / 2, -0.5};
			imr_push_quad_tex(
				&imr,
				pos,
				size,
				(Rect) { 0, 0, 1, 1 },
				bg_2.id,
				rotate_z(0),
				(v4) { 1, 1, 1, 1 }
			);
		}

		for (i32 i = -2; i < 2; i++) {
			v3 pos = { i * size.x - size.x / 2, 0 - size.y / 2, 0.5};
			imr_push_quad_tex(
				&imr,
				pos,
				size,
				(Rect) { 0, 0, 1, 1 },
				bg_3.id,
				rotate_z(0),
				(v4) { 1, 1, 1, 1 }
			);
		}

		// Ground
		v4 color = { 1, 1, 1, 1 };
		imr_push_triangle_tex(
			&imr,
			(v3) {-3.75 + 0, -0.5, 0.5},
			(v3) {-3.75 + 0, -0.5, 2.5},
			(v3) {-3.75 + 6, -0.5, 0.5},
			(Triangle) {
				(v3) { 0, 1, 0 },
				(v3) { 0, 0, 0 },
				(v3) { 1, 1, 0 },
			},
			ground.id,
			rotate_z(0),
			color
		);
		imr_push_triangle_tex(
			&imr,
			(v3) {-3.75 + 0, -0.5, 2.5},
			(v3) {-3.75 + 6, -0.5, 2.5},
			(v3) {-3.75 + 6, -0.5, 0.5},
			(Triangle) {
				(v3) { 0, 0, 0 },
				(v3) { 1, 0, 0 },
				(v3) { 1, 1, 0 },
			},
			ground.id,
			rotate_z(0),
			color
		);

		// Fence
		imr_push_quad_tex(
			&imr,
			(v3) { -0.6, -0.5, 0.6 },
			(v2) { 0.3, 0.1 },
			(Rect) { 0, 0, 1, 1 },
			fence.id,
			rotate_z(0),
			color
		);
		imr_push_quad_tex(
			&imr,
			(v3) { -0.3, -0.5, 0.6 },
			(v2) { 0.3, 0.1 },
			(Rect) { 0, 0, 1, 1 },
			fence.id,
			rotate_z(0),
			color
		);

		imr_push_quad_tex(
			&imr,
			(v3) { 0.5, -0.5, 0.6 },
			(v2) { 0.3, 0.1 },
			(Rect) { 0, 0, 1, 1 },
			fence.id,
			rotate_z(0),
			color
		);
		imr_push_quad_tex(
			&imr,
			(v3) { 0.8, -0.5, 0.6 },
			(v2) { 0.3, 0.1 },
			(Rect) { 0, 0, 1, 1 },
			fence.id,
			rotate_z(0),
			color
		);

		// Lamp
		imr_push_quad_tex(
			&imr,
			(v3) { -0.8, -0.5, 0.6 },
			(v2) { 0.1, 0.3 },
			(Rect) { 0, 0, 1, 1 },
			lamp.id,
			rotate_z(0),
			color
		);

		// Shop
		imr_push_quad_tex(
			&imr,
			(v3) { 0.0, -0.5, 0.6 },
			(v2) { 0.5, 0.4 },
			(Rect) { 0, 0, 1, 1 },
			shop.id,
			rotate_z(0),
			color
		);

		// Player
		imr_push_quad_tex(
			&imr,
			(v3) { -0.5, -0.5, 1 },
			(v2) { 0.3, 0.3 },
			(Rect) { 0, 6.0/7, 1.0f/8, 1.0f/7 },
			pl_tex.id,
			rotate_z(0),
			(v4) { 1, 1, 1, 1 }
		);


		// Fence
		imr_push_quad_tex(
			&imr,
			(v3) { -0.9, -0.5, 1.1 },
			(v2) { 0.3, 0.1 },
			(Rect) { 0, 0, 1, 1 },
			fence.id,
			rotate_z(0),
			color
		);
		imr_push_quad_tex(
			&imr,
			(v3) { -0.6, -0.5, 1.1 },
			(v2) { 0.3, 0.1 },
			(Rect) { 0, 0, 1, 1 },
			fence.id,
			rotate_z(0),
			color
		);

		// Rock
		imr_push_quad_tex(
			&imr,
			(v3) { 0.5, -0.5, 1.2 },
			(v2) { 0.2, 0.1 },
			(Rect) { 0, 0, 1, 1 },
			rock_3.id,
			rotate_z(0),
			color
		);
		imr_push_quad_tex(
			&imr,
			(v3) { 0.4, -0.5, 1.3 },
			(v2) { 0.2, 0.1 },
			(Rect) { 0, 0, 1, 1 },
			rock_3.id,
			rotate_z(0),
			color
		);

		// Lamp
		imr_push_quad_tex(
			&imr,
			(v3) { 0.7, -0.5, 1.2 },
			(v2) { 0.1, 0.3 },
			(Rect) { 0, 0, 1, 1 },
			lamp.id,
			rotate_z(0),
			color
		);

		imr_end(&imr);

		window_update(&window);
	}
	return 0;
}

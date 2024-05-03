#include <stdio.h>
#include "window/window.h"
#include "graphics/imr.h"
#include "graphics/texture.h"
#include "camera/camera.h"
#include "event/event.h"
#include "math/utils.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define TILE_WIDTH 32
#define TILE_HEIGHT 32
#define ROW 7
#define COL 7

typedef struct {
	i32 width, height;
	i32 x_offset, y_offset;
} TileConfig;

typedef enum {
	GRASS,
	DIRT,
	WATER
} TileType;

const TileType map[ROW][COL] = {
	{ 0, 0, 1, 1, 1, 0, 0 },
	{ 0, 1, 1, 2, 1, 1, 1 },
	{ 0, 1, 2, 2, 2, 2, 1 },
	{ 0, 1, 2, 2, 2, 1, 0 },
	{ 0, 1, 2, 2, 1, 0, 0 },
	{ 0, 0, 1, 1, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0 },
};

f32 area(v2 a, v2 b, v2 c) {
	return 0.5 * abs(
		a.x * (b.y - c.y) +
		b.x * (c.y - a.y) +
		c.x * (a.y - b.y)
	);
}

v2 pixel_to_gl_coords(v2 pos) {
	return (v2) {
		(2 * pos.x) / WIN_WIDTH - 1,
		1 - (2 * pos.y) / WIN_HEIGHT
	};
}

v2 gl_to_pixel_coords(v2 pos) {
	return (v2) {
		(pos.x + 1) * WIN_WIDTH / 2,
		(1 - pos.y) * WIN_HEIGHT / 2
	};
}

typedef struct {
	v2 a, b, c, d;
} Polygon;
Polygon pol_map[ROW][COL];

int main(int argc, char** argv) {
	Window window = Result_Window_unwrap(window_new("Game", WIN_WIDTH, WIN_HEIGHT));
	IMR imr = Result_IMR_unwrap(imr_new());
	OCamera cam = ocamera_new(
		(v2) { 0, 0 },
		3.0f,
		(OCamera_Boundary) { 0, WIN_WIDTH, 0, WIN_HEIGHT, -1, 1000 }
	);

	Texture tex = Result_Texture_unwrap(texture_from_file("assets/sprites.png", false));
	texture_bind(tex);

	TileConfig tconf = {
		.width = 32, .height = 32,
		.x_offset = 16, .y_offset = 8
	};

	while (!window.should_close) {
		Event event;
		while(event_poll(window, &event)) {
			if (event.type == KEYDOWN) {
				switch (event.e.key) {
					case GLFW_KEY_EQUAL:
						ocamera_change_zoom(&cam, 0.1);
						break;
					case GLFW_KEY_MINUS:
						ocamera_change_zoom(&cam, -0.1);
						break;

					case GLFW_KEY_W:
						ocamera_change_pos(&cam, (v2) { 0, 0.1 });
						break;
					case GLFW_KEY_A:
						ocamera_change_pos(&cam, (v2) { -0.1, 0 });
						break;
					case GLFW_KEY_S:
						ocamera_change_pos(&cam, (v2) { 0, -0.1 });
						break;
					case GLFW_KEY_D:
						ocamera_change_pos(&cam, (v2) { 0.1, 0 });
						break;
				}
			}
		}

		m4 mvp = ocamera_calc_mvp(&cam);
		imr_update_mvp(&imr, mvp);

		imr_clear((v4) { 0, 0, 0, 1 });
		imr_begin(&imr);

		// Tile rendering
		for (i32 y = 0; y < ROW; y++) {
			for (i32 x = 0; x < COL; x++) {
				i32 px = (x - y) * (tconf.width / 2);
				i32 py = (x + y) * (tconf.height / 2 - tconf.y_offset);
				imr_push_quad(
					&imr,
					(v2) { px, py },
					(v2) { tconf.width , tconf.height },
					(Rect) { map[y][x] / 3.0f, 0, 1.0f / 3.0f, 1 },
					tex.id,
					rotate_z(0),
					(v4) { 1, 1, 1, 1 }
				);

				pol_map[y][x] = (Polygon) {
					(v2) { px + tconf.width / 2, py + tconf.y_offset },
					(v2) { px, py + tconf.height / 2 },
					(v2) { px + tconf.width / 2 , py + tconf.height - tconf.y_offset },
					(v2) { px + tconf.width, py + tconf.height / 2 }
				};
			}
		}

		// Tile selection
		v2 p = event_mouse_pos(window);
		v2 cp = gl_to_pixel_coords(cam.pos);
		p.x += floor(cp.x - WIN_WIDTH / 2);
		p.y += floor(cp.y - WIN_HEIGHT / 2);
		p.x = floor(p.x / WIN_WIDTH * cam.boundary.right);
		p.y = floor(p.y / WIN_HEIGHT * cam.boundary.bottom);

		for (i32 y = 0; y < ROW; y++) {
			for (i32 x = 0; x < COL; x++) {
				Polygon pol = pol_map[y][x];
				f32 A = area(pol.a, pol.b, pol.c) + area(pol.c, pol.d, pol.a);
				f32 A1 = area(pol.a, p, pol.b);
				f32 A2 = area(pol.a, p, pol.d);
				f32 A3 = area(pol.c, p, pol.b);
				f32 A4 = area(pol.c, p, pol.d);
				if (A1 + A2 + A3 + A4 == A) {
					i32 px = (x - y) * (tconf.width / 2);
					i32 py = (x + y) * (tconf.height / 2 - tconf.y_offset);
					imr_push_quad(
						&imr,
						(v2) { px, py },
						(v2) { tconf.width , tconf.height },
						(Rect) { map[y][x] / 3.0f, 0, 1.0f / 3.0f, 1 },
						tex.id,
						rotate_z(0),
						(v4) { 1, 0, 0, 0.5 }
					);
				}
			}
		}

		imr_end(&imr);
		window_update(&window);
	}

	texture_delete(tex);
	imr_delete(&imr);
	window_delete(window);
	return 0;
}

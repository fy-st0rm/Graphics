#include <stdio.h>
#include "window/window.h"
#include "graphics/imr.h"
#include "graphics/texture.h"
#include "camera/camera.h"
#include "event/event.h"

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

int main(int argc, char** argv) {
	Window window = Result_Window_unwrap(window_new("Game", WIN_WIDTH, WIN_HEIGHT));
	IMR imr = Result_IMR_unwrap(imr_new());
	OCamera cam = ocamera_new(
		(v2) { 0, 0 },
		2.0f,
		(OCamera_Boundary) { 0, WIN_WIDTH, WIN_HEIGHT, 0, -1, 1000 }
	);

	Texture tex = Result_Texture_unwrap(texture_from_file("assets/sprites.png"));
	texture_bind(tex);

	TileConfig tconf = {
		.width = 32, .height = 32,
		.x_offset = 16, .y_offset = 8
	};

	while (!window.should_close) {
		Event event;
		while(event_poll(window, &event)) {
			if (event.type == KEYDOWN) {
				switch (event.key) {
					case GLFW_KEY_EQUAL:
						ocamera_change_zoom(&cam, 0.1);
						break;
					case GLFW_KEY_MINUS:
						ocamera_change_zoom(&cam, -0.1);
						break;
				}
			}
		}

		m4 mvp = ocamera_calc_mvp(&cam);
		imr_update_mvp(&imr, mvp);

		imr_clear((v4) { 0, 0, 0, 1 });
		imr_begin(&imr);
		for (i32 y = ROW - 1; y >= 0; y--) {
			for (i32 x = 0, i = 0, j = 0; x < COL; x++, i++, j++) {
				i32 px = 140 + x * tconf.width - i * tconf.x_offset - (ROW - 1 - y) * tconf.x_offset;
				i32 py = y * tconf.height - j * tconf.y_offset + (ROW - 1 - y) * (tconf.height - tconf.y_offset);
				imr_push_quad(
					&imr,
					(v2) { px, py },
					(v2) { tconf.width , tconf.height },
					(Rect) { map[y][x] / 3.0f, 0, 1.0f / 3.0f, 1 },
					tex.id,
					rotate_z(0),
					(v4) { 1, 1, 1, 1 }
				);
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

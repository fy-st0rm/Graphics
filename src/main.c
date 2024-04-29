#include <stdio.h>
#include "window/window.h"
#include "graphics/imr.h"
#include "graphics/texture.h"
#include "camera/camera.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define TILE_WIDTH 32
#define TILE_HEIGHT 32
#define ROW 7
#define COL 7
const i32 map[ROW][COL] = {
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
	f32 zoom = 2.1f;
	OCamera cam = ocamera_new(
		(v2) { 0, 0 },
		(OCamera_Boundary) { 0 / zoom, WIN_WIDTH / zoom, WIN_HEIGHT / zoom, 0 / zoom, -1, 1000 }
	);

	Texture tex = Result_Texture_unwrap(texture_from_file("assets/sprites.png"));
	texture_bind(tex);

	while (!window.should_close) {
		m4 mvp = ocamera_calc_mvp(&cam);
		imr_update_mvp(&imr, mvp);
		imr_begin(&imr);
		for (i32 y = ROW - 1; y >= 0; y--) {
			for (i32 x = 0, i = 0, j = 0; x < COL; x++, i++, j++) {
				i32 px = 140 + x * TILE_WIDTH - i * 16 - (ROW - 1 - y) * 16;
				i32 py = y * TILE_HEIGHT - j * 8 + (ROW - 1 - y) * 24;
				imr_push_quad(
					&imr,
					(v2) { px, py },
					(v2) { TILE_WIDTH, TILE_HEIGHT },
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

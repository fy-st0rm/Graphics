#include <stdio.h>
#include "window/window.h"
#include "graphics/imr.h"
#include "graphics/texture.h"

int main(int argc, char** argv) {
	Window window = Result_Window_unwrap(window_new("Game", 800, 600));
	IMR imr = Result_IMR_unwrap(imr_new());

	Texture tex = Result_Texture_unwrap(texture_from_file("assets/tiles.png"));
	texture_bind(tex);

	while (!window.should_close) {
		imr_begin(&imr);
		imr_push_quad(
			&imr,
			(v2) { -0.7, -0.7 },
			(v2) { 1.5, 1.2 },
			(Rect) { 0, 0, 1, 1 },
			tex.id,
			rotate_z(0),
			(v4) { 1, 1, 1, 1 }
		);
		//imr_push_triangle(
		//	&imr,
		//	(v2) {  0.5,  0.3 },
		//	(v2) {  0.2, -0.3 },
		//	(v2) {  0.8, -0.3 },
		//	rotate_z(0),
		//	(v4) { 0, 1, 1, 0.5 }
		//);
		imr_end(&imr);
		window_update(&window);
	}

	texture_delete(tex);
	imr_delete(&imr);
	window_delete(window);
	return 0;
}

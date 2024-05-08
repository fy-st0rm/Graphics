#include "window/window.h"
#include "graphics/imr.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

int main() {
	Window window = Result_Window_unwrap(
		window_new("3D", WIN_WIDTH, WIN_HEIGHT)
	);
	IMR imr = Result_IMR_unwrap(imr_new());

	while (!window.should_close) {
		imr_clear((v4) { 0, 0.5, 0.5, 1 });
		window_update(&window);
	}
	return 0;
}

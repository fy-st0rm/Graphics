#include "window/window.h"
#include "graphics/imr.h"
#include "camera/camera.h"
#include "event/event.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

int main() {
	Window window = Result_Window_unwrap(
		window_new("3D", WIN_WIDTH, WIN_HEIGHT)
	);
	IMR imr = Result_IMR_unwrap(imr_new());

	PCamera cam = pcamera_new(
		(v3) { 0, 0, 6 },
		(v3) { 0, 0, -1 },
		(PCamera_Info) {
			.aspect_ratio = (f32) WIN_WIDTH / WIN_HEIGHT,
			.fov = 45.0f,
			.near = 1.0f,
			.far = 1000.0f
		}
	);

	while (!window.should_close) {
		Event event;
		while(event_poll(window, &event)) {
			if (event.type == KEYDOWN) {
				switch (event.e.key) {
					case GLFW_KEY_EQUAL:
						pcamera_change_pos(&cam, (v3) {0, 0, -0.1});
						break;
					case GLFW_KEY_MINUS:
						pcamera_change_pos(&cam, (v3) {0, 0, 0.1});
						break;
				}
			}
		}

		m4 mvp = pcamera_calc_mvp(&cam);
		imr_update_mvp(&imr, mvp);

		imr_clear((v4) { 0, 0.5, 0.5, 1 });
		imr_begin(&imr);

		imr_push_quad(
			&imr,
			(v3) { 0, 0, 0 },
			(v2) { 0.5, 0.5 },
			rotate_z(0),
			(v4) { 1, 0, 0, 1 }
		);

		imr_end(&imr);

		window_update(&window);
	}
	return 0;
}

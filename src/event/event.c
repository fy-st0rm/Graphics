#include "event.h"

static Dyn_Array(Event) events = NULL;

void key_callback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
	Event event = { 0 };
	event.key = key;
	switch (action) {
		case GLFW_PRESS:
		case GLFW_REPEAT:
			event.type = KEYDOWN;
			break;
		case GLFW_RELEASE:
			event.type = KEYUP;
			break;
	}

	dyn_array_append(events, event);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	Event event = {0};

	switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			event.button = MOUSE_BUTTON_LEFT;
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			event.button = MOUSE_BUTTON_RIGHT;
			break;
		default:
			assert(0, "Unhandled mouse button: %d\n", button);
	}

	switch (action) {
		case GLFW_PRESS:
			event.type = MOUSE_BUTTON_DOWN;
			break;
		case GLFW_RELEASE:
			event.type = MOUSE_BUTTON_UP;
			break;
		default:
			assert(0, "Unhandled mouse action: %d\n", action);
	}

	dyn_array_append(events, event);
}

i32 event_poll(Window window, Event* event) {
	glfwSetKeyCallback(window.glfw_window, key_callback);
	glfwSetMouseButtonCallback(window.glfw_window, mouse_button_callback);

	i32 len = dyn_array_len(events);
	if (len)
		*event = dyn_array_pop(events, 0);
	return len;
}

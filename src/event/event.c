#include "event.h"
#include "core/ctx.h"

extern Context* ctx;

void key_callback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
	Event event = { 0 };
	event.e.key = key;
	switch (action) {
		case GLFW_PRESS:
		case GLFW_REPEAT:
			event.type = KEYDOWN;
			break;
		case GLFW_RELEASE:
			event.type = KEYUP;
			break;
	}

	dyn_array_append(ctx->events, event);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	Event event = {0};

	switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			event.e.button = MOUSE_BUTTON_LEFT;
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			event.e.button = MOUSE_BUTTON_RIGHT;
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

	dyn_array_append(ctx->events, event);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	Event event = { 0 };
	event.e.mouse_pos = (v2) { xpos, ypos };
	event.type = MOUSE_MOTION;
	dyn_array_append(ctx->events, event);
}

i32 event_poll(Window window, Event* event) {
	glfwSetKeyCallback(window.glfw_window, key_callback);
	glfwSetMouseButtonCallback(window.glfw_window, mouse_button_callback);
	glfwSetCursorPosCallback(window.glfw_window, cursor_position_callback);

	i32 len = dyn_array_len(ctx->events);
	if (len)
		*event = dyn_array_pop(ctx->events, 0);
	return len;
}

v2 event_mouse_pos(Window window) {
	f64 xpos, ypos;
	glfwGetCursorPos(window.glfw_window, &xpos, &ypos);
	return (v2) {
		xpos, ypos
	};
}

void event_set_mouse_pos(Window window, v2 pos) {
	glfwSetCursorPos(window.glfw_window, pos.x, pos.y);
}

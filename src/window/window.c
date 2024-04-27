#include "window.h"

DEFINE_RESULT(Window, Window);

Result_Window window_new(const char* title, u32 width, u32 height) {
	if (!glfwInit())
		return Result_Window_err("Failed to initialize glfw");

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* glfw_window = glfwCreateWindow(
		width, height,
		title, NULL, NULL
	);

	if (!glfw_window)
		return Result_Window_err("Failed to create glfw window");

	glfwMakeContextCurrent(glfw_window);

	if (glewInit() != GLEW_OK)
		return Result_Window_err("Failed to initialize glew");

	b8 should_close = glfwWindowShouldClose(glfw_window);

	return Result_Window_ok((Window) {
		.glfw_window = glfw_window,
		.width = width,
		.height = height,
		.should_close = should_close
	});
}

void window_delete(Window window) {
	glfwDestroyWindow(window.glfw_window);
}

void window_update(Window* window) {
	window->should_close = glfwWindowShouldClose(window->glfw_window);
	glfwSwapBuffers(window->glfw_window);
	glfwPollEvents();
}

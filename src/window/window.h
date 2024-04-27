#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "core/defines.h"
#include "core/result.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"

typedef struct {
	GLFWwindow* glfw_window;
	u32 width, height;
	b8 should_close;
} Window;

DECLARE_RESULT(Window, Window);

Result_Window window_new(const char* title, u32 width, u32 height);
void window_delete(Window window);
void window_update(Window* window);

#endif // __WINDOW_H__

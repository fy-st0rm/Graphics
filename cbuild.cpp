#define CBUILD_IMPLEMENTATION
#include "cbuild.h"

int main(int argc, char** argv) {
	cbuild_rebuild(argc, argv);
	CBuild cbuild("gcc");
	cbuild
		.out("bin", "game")
		.flags({"-std=gnu17"})
		.inc_paths({
			"src/",
			"src/external/glew/include/",
			"src/external/glfw/include/",
			"src/external/stb/"
		})
#ifdef _WIN32
		.libs({"mingw32", "glu32", "opengl32", "User32", "Gdi32", "Shell32", "m"})
#elif defined(__linux__)
		.libs({"GL", "GLU", "m"})
#endif
		.src({
			"src/external/glfw/src/glfw.c",
			"src/external/glew/src/glew.c",
			"src/external/stb/stb_image.c",
			"src/core/defines.c",
			"src/math/vec.c",
			"src/math/mat.c",
			"src/graphics/shader.c",
			"src/graphics/texture.c",
			"src/graphics/imr.c",
			"src/event/event.c",
			"src/camera/camera.c",
			"src/window/window.c",
			"src/main.c",
		})
		.build()
		.clean()
		.run(argv);

	return 0;
}

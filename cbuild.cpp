#define CBUILD_IMPLEMENTATION
#include "cbuild.h"

// TODO: Cache engine into objects

void compile_engine() {
	CBuild cbuild("gcc");
	cbuild
		.flags({"-std=gnu17"})
		.inc_paths({
			"src/",
			"src/external/glew/include/",
			"src/external/glfw/include/",
			"src/external/stb/"
		})
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
		})
		.compile();
}

void build_iso(int argc, char** argv) {
	compile_engine();

	CBuild cbuild("gcc");
	cbuild
		.out("bin", "iso")
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
			"src/examples/iso.c",
		})
		.objs({
			"src/external/glfw/src/glfw.o",
			"src/external/glew/src/glew.o",
			"src/external/stb/stb_image.o",
			"src/core/defines.o",
			"src/math/vec.o",
			"src/math/mat.o",
			"src/graphics/shader.o",
			"src/graphics/texture.o",
			"src/graphics/imr.o",
			"src/event/event.o",
			"src/camera/camera.o",
			"src/window/window.o",
		})
		.build()
		.clean()
		.run(argv);
}

void build_3d(int argc, char** argv) {
	compile_engine();

	CBuild cbuild("gcc");
	cbuild
		.out("bin", "3d")
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
			"src/examples/3d.c",
		})
		.objs({
			"src/external/glfw/src/glfw.o",
			"src/external/glew/src/glew.o",
			"src/external/stb/stb_image.o",
			"src/core/defines.o",
			"src/math/vec.o",
			"src/math/mat.o",
			"src/graphics/shader.o",
			"src/graphics/texture.o",
			"src/graphics/imr.o",
			"src/event/event.o",
			"src/camera/camera.o",
			"src/window/window.o",
		})
		.build()
		.clean()
		.run(argv);
}

void print_usage() {
	std::cout << "[Usage]: ./cbuild [options]" << std::endl;
	std::cout << "\tiso: Builds isometric example\n";
	std::cout << "\t3d: Builds 3D example\n";
}

int main(int argc, char** argv) {
	cbuild_rebuild(argc, argv);
	std::vector<std::string> args(argv, argv + argc);

	args.erase(args.begin());

	if (!args.size())
		print_usage();

	for (const std::string& arg : args) {
		if (arg == "iso")
			build_iso(argc, argv);
		else if (arg == "3d")
			build_3d(argc, argv);
		else
			print_usage();
	}
	return 0;
}

#define CBUILD_IMPLEMENTATION
#include "cbuild.h"

// TODO: Cache engine into objects

std::vector<std::string> compile_engine() {
	CBuild cbuild("gcc");
	cbuild
		.flags({
			"-D_GNU_SOURCE"
		})
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

	return cbuild.get_objs();
}

void build_iso(int argc, char** argv) {
	auto objs = compile_engine();

	CBuild cbuild("gcc");
	cbuild
		.out("bin", "iso")
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
		.objs(objs)
		.build()
		.clean()
		.run(argv);
}

void build_2d(int argc, char** argv) {
	auto objs = compile_engine();

	CBuild cbuild("gcc");
	cbuild
		.out("bin", "2d")
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
			"src/examples/2d.c",
		})
		.objs(objs)
		.build()
		.clean()
		.run(argv);
}

void print_usage() {
	std::cout << "[Usage]: ./cbuild [options]" << std::endl;
	std::cout << "\tiso: Builds isometric example\n";
	std::cout << "\t2d: Builds 2D example\n";
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
		else if (arg == "2d")
			build_2d(argc, argv);
		else
			print_usage();
	}
	return 0;
}

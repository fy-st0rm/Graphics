#define CBUILD_IMPLEMENTATION
#include "cbuild.h"

void build_engine() {
	CBuild cbuild("gcc");
	cbuild
		.out("bin", "libengine.a")
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
			"src/core/trace_allocator.c",
			"src/core/ctx.c",
			"src/core/alloc.c",
			"src/math/vec.c",
			"src/math/mat.c",
			"src/graphics/shader.c",
			"src/graphics/texture.c",
			"src/graphics/fbo.c",
			"src/graphics/imr.c",
			"src/ecs/ecs.c",
			"src/event/event.c",
			"src/camera/camera.c",
			"src/window/window.c",
		})
		.build_static_lib()
		.clean();
}

void build_iso(int argc, char** argv) {
	CBuild cbuild("gcc");
	cbuild
		.out("bin", "iso")
		.inc_paths({
			"src/",
			"src/external/glew/include/",
			"src/external/glfw/include/",
			"src/external/stb/"
		})
		.lib_paths({
			"bin/",
		})
#ifdef _WIN32
		.libs({"mingw32", "enigne", "glu32", "opengl32", "User32", "Gdi32", "Shell32", "m"})
#elif defined(__linux__)
		.libs({"engine", "GL", "GLU", "m"})
#endif
		.src({
			"src/examples/iso.c",
		})
		.build()
		.clean()
		.run(argv);
}

void build_2d(int argc, char** argv) {
	CBuild cbuild("gcc");
	cbuild
		.out("bin", "2d")
		.inc_paths({
			"src/",
			"src/external/glew/include/",
			"src/external/glfw/include/",
			"src/external/stb/"
		})
		.lib_paths({
			"bin/",
		})
#ifdef _WIN32
		.libs({"mingw32", "enigne", "glu32", "opengl32", "User32", "Gdi32", "Shell32", "m"})
#elif defined(__linux__)
		.libs({"engine", "GL", "GLU", "m"})
#endif
		.src({
			"src/examples/2d.c",
		})
		.build()
		.clean()
		.run(argv);
}

void build_light(int argc, char** argv) {
	CBuild cbuild("gcc");
	cbuild
		.out("bin", "light")
		.inc_paths({
			"src/",
			"src/external/glew/include/",
			"src/external/glfw/include/",
			"src/external/stb/"
		})
		.lib_paths({
			"bin/",
		})
#ifdef _WIN32
		.libs({"mingw32", "enigne", "glu32", "opengl32", "User32", "Gdi32", "Shell32", "m"})
#elif defined(__linux__)
		.libs({"engine", "GL", "GLU", "m"})
#endif
		.src({
			"src/examples/light.c",
		})
		.build()
		.clean()
		.run(argv);
}

void build_game(int argc, char** argv) {
	CBuild cbuild("gcc");
	cbuild
		.out("bin", "game")
		.inc_paths({
			"src/",
			"src/external/glew/include/",
			"src/external/glfw/include/",
			"src/external/stb/"
		})
		.lib_paths({
			"bin/",
		})
#ifdef _WIN32
		.libs({"mingw32", "enigne", "glu32", "opengl32", "User32", "Gdi32", "Shell32", "m"})
#elif defined(__linux__)
		.libs({"engine", "GL", "GLU", "m"})
#endif
		.src({
			"src/game/renderer.c",
			"src/game/main.c",
		})
		.build()
		.clean()
		.run(argv);
}

void print_usage() {
	std::cout << "[Usage]: ./cbuild [options]" << std::endl;
	std::cout << "\tengine: Builds engine\n";
	std::cout << "\tiso: Builds isometric example\n";
	std::cout << "\t2d: Builds 2D example\n";
	std::cout << "\tlight: Builds light example\n";
	std::cout << "\tgame: Builds game\n";
}

int main(int argc, char** argv) {
	cbuild_rebuild(argc, argv);
	std::vector<std::string> args(argv, argv + argc);

	args.erase(args.begin());

	if (!args.size())
		print_usage();

	for (const std::string& arg : args) {
		if (arg == "engine")
			build_engine();
		else if (arg == "iso")
			build_iso(argc, argv);
		else if (arg == "2d")
			build_2d(argc, argv);
		else if (arg == "light")
			build_light(argc, argv);
		else if (arg == "game")
			build_game(argc, argv);
		else
			print_usage();
	}
	return 0;
}

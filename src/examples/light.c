#include <stdio.h>

#include "window/window.h"
#include "graphics/imr.h"
#include "camera/camera.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

const char* vertex_src =
	"#version 440 core\n"
	"layout (location = 0) in vec3 position;\n"
	"layout (location = 1) in vec4 color;\n"
	"layout (location = 2) in vec2 tex_coord;\n"
	"layout (location = 3) in float tex_id;\n"
	"uniform mat4 mvp;\n"
	"out vec4 o_color;\n"
	"out vec2 o_tex_coord;\n"
	"out float o_tex_id;\n"
	"void main() {\n"
	"o_color = color;\n"
	"o_tex_coord = tex_coord;\n"
	"o_tex_id = tex_id;\n"
	"gl_Position = mvp * vec4(position, 1.0f);\n"
	"}\n";

const char* fragment_src =
	"#version 440 core\n"
	"layout (location = 0) out vec4 color;\n"
	"uniform sampler2D textures[32];\n"
	"in vec4 o_color;\n"
	"in vec2 o_tex_coord;\n"
	"in float o_tex_id;\n"
	"void main() {\n"
	"vec2 st = gl_FragCoord.xy / vec2(800, 600);\n"
	"float p = 1 - smoothstep(-0.2, 0.5, distance(st, vec2(0.5)));\n"
	"color = o_color * p;\n"
	"}\n";

int main(int argc, char** argv) {
	// Window
	Window window = Result_Window_unwrap(
		window_new("Light", WIN_WIDTH, WIN_HEIGHT)
	);

	// Creating custom shader
	Shader shader = Result_Shader_unwrap(shader_new(vertex_src, fragment_src));

	// Providing texture samples
	u32 samplers[TEXTURE_SAMPLE_AMT];
	for (u32 i = 0; i < TEXTURE_SAMPLE_AMT; i++)
		samplers[i] = i;
	
	// Providing samplers to the shader
	//GLCall(glUseProgram(shader));
	//int loc = GLCall(glGetUniformLocation(shader, "textures"));
	//assert(loc != -1, "Cannot find uniform: textures\n");
	//GLCall(glUniform1iv(loc, TEXTURE_SAMPLE_AMT, samplers));

	// Renderer
	IMR imr = Result_IMR_unwrap(imr_new());
	imr_switch_shader(&imr, shader);

	// Camera
	OCamera cam = ocamera_new(
		(v2) { 0, 0 },
		1.0f,
		(OCamera_Boundary) { 0, WIN_WIDTH, 0, WIN_HEIGHT, -1, 1000 }
	);

	while (!window.should_close) {
		imr_clear((v4) { 0, 0, 0, 1 });

		m4 mvp = ocamera_calc_mvp(&cam);
		imr_update_mvp(&imr, mvp);

		imr_begin(&imr);
		imr_push_quad(
			&imr,
			(v3) { 0, 0, 0 },
			(v2) { 800, 600},
			rotate_z(0),
			(v4) { 1, 1, 1, 1 }
		);

		imr_push_quad(
			&imr,
			(v3) { 400, 300, 0 },
			(v2) { 50, 50},
			rotate_z(0),
			(v4) { 1, 0, 1, 1 }
		);
		imr_end(&imr);

		window_update(&window);
	}

	window_delete(window);
	return 0;
}

#include <stdio.h>

#include "window/window.h"
#include "graphics/imr.h"
#include "camera/camera.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define SURF_WIDTH 133
#define SURF_HEIGHT 100

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
	"float intensity = 0.9;\n"
	"vec2 dim = vec2(266, 200);\n"
	"vec2 light_pos = vec2(133 / 2, 100 / 2);\n"
	"vec2 light_norm = light_pos / dim;\n"
	"vec2 pix_size = vec2(1);\n"
	"void main() {\n"
	"vec2 block_coord = floor(gl_FragCoord.xy / pix_size) * pix_size;\n"
	"vec2 uv = block_coord / dim;\n"
	"float falloff = pow(1 - distance(light_norm, uv.xy), 2);\n"
	"float angle = abs(atan(uv.y - light_norm.y, uv.x - light_norm.x));\n"
	"float angfalloff = smoothstep(3.14 / 2, 0, angle);\n"
	"color = o_color * intensity * falloff * angfalloff;\n"
	//"color = o_color;\n"
	"}\n";

//const char* fragment_src =
//	"#version 440 core\n"
//	"layout (location = 0) out vec4 color;\n"
//	"uniform sampler2D textures[32];\n"
//	"vec2 light = vec2(200, 100);\n"
//	"vec4 amb = vec4(0.02, 0.02, 0.02, 1);\n"
//	"in vec4 o_color;\n"
//	"in vec2 o_tex_coord;\n"
//	"in float o_tex_id;\n"
//	"void main() {\n"
//	"vec2 normal =  vec2(133, 100) - gl_FragCoord.xy;\n"
//	"vec2 dir = normalize(gl_FragCoord.xy - light);\n"
//	"float a = clamp(dot(dir, normal), 0.0, 1);\n"
//	"vec2 st = gl_FragCoord.xy / vec2(266, 200);\n"
//	"float p = pow(1 - distance(st, light / vec2(266, 200)), 2);\n"
//	"color = o_color * p * a + amb;\n"
//	//"color = o_color;\n"
//	"}\n";

//const char* fragment_src =
//	"#version 440 core\n"
//	"layout (location = 0) out vec4 color;\n"
//	"uniform sampler2D textures[32];\n"
//	"in vec4 o_color;\n"
//	"in vec2 o_tex_coord;\n"
//	"in float o_tex_id;\n"
//	"void main() {\n"
//	"vec2 st = gl_FragCoord.xy / vec2(800, 600);\n"
//	"float p = 1 - smoothstep(-0.2, 0.5, distance(st, vec2(0.5)));\n"
//	"color = o_color * p;\n"
//	"}\n";

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

	// Camera
	OCamera cam = ocamera_new(
		(v2) { 0, 0 },
		1.0f,
		(OCamera_Boundary) { 0, SURF_WIDTH, SURF_HEIGHT, 0, -1, 1000 }
	);

	OCamera cam2 = ocamera_new(
		(v2) { 0, 0 },
		1.0f,
		(OCamera_Boundary) { 0, WIN_WIDTH, WIN_HEIGHT, 0, -1, 1000 }
	);

	GLuint framebuffer, colorTexture, depthRenderbuffer;
	
	// Generate and bind framebuffer
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// Create and attach color texture
	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SURF_WIDTH, SURF_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

	// Create and attach depth renderbuffer
	//glGenRenderbuffers(1, &depthRenderbuffer);
	//glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 600);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebuffer is not complete!\n");
	}

	while (!window.should_close) {

		// Color pass
		{
			imr_switch_shader(&imr, shader);

			m4 mvp = ocamera_calc_mvp(&cam);
			imr_update_mvp(&imr, mvp);

			glViewport(0, 0, SURF_WIDTH, SURF_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);


			imr_clear((v4) { 0, 0, 0, 1 });

			imr_begin(&imr);
			imr_update_mvp(&imr, mvp);

			v2 size = { 200, 200 };
			imr_push_quad(
				&imr,
				(v3) {
					SURF_WIDTH / 2 - size.x / 2,
					SURF_HEIGHT / 2 - size.y / 2,
					0
				},
				size,
				rotate_z(0),
				(v4) { 1, 1, 1, 1 }
			);

			//imr_push_quad(
			//	&imr,
			//	(v3) { 350, 300, 0 },
			//	(v2) { 50, 50},
			//	rotate_z(0),
			//	(v4) { 1, 0, 1, 1 }
			//);
			imr_end(&imr);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		// Final render
		{
			imr_switch_shader_to_default(&imr);

			m4 mvp = ocamera_calc_mvp(&cam2);
			imr_update_mvp(&imr, mvp);

			glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
			GLCall(glBindTextureUnit(colorTexture, colorTexture));

			imr_clear((v4) { 0, 0, 0, 1 });
			
			imr_begin(&imr);
			imr_update_mvp(&imr, mvp);

			imr_push_quad_tex(
				&imr,
				(v3) { 0, 0, 0 },
				(v2) { 800, 600},
				(Rect) { 0, 0, 1, 1 },
				colorTexture,
				rotate_z(0),
				(v4) { 1, 1, 1, 1 }
			);
			imr_end(&imr);
		}

		window_update(&window);
	}

	window_delete(window);
	return 0;
}

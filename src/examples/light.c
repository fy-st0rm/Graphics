#include <stdio.h>

#include "window/window.h"
#include "graphics/imr.h"
#include "camera/camera.h"
#include "math/utils.h"

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

	"layout (location = 0) out vec4 color_channel;\n"
	"layout (location = 1) out vec4 normal_channel;\n"

	"in vec4 o_color;\n"
	"in vec2 o_tex_coord;\n"
	"in float o_tex_id;\n"

	"uniform vec2 light_pos;\n"
	"uniform vec2 dim;\n"

	"vec2 pos = vec2(133 / 2, 100 / 2) / dim;\n"

	"float intensity = 0.9;\n"
	"vec2 light_norm = light_pos / dim;\n"
	"vec2 pix_size = vec2(2);\n"

	"void main() {\n"

		"vec2 block_coord = floor(gl_FragCoord.xy / pix_size) * pix_size;\n"
		"vec2 uv = block_coord / dim;\n"

		"float radial_fall_off = pow(1 - distance(light_norm, uv.xy), 2);\n"
		"float angle = abs(atan(uv.y - light_norm.y, uv.x - light_norm.x));\n"
		"float angular_fall_off = smoothstep(3.14 / 2, 0, angle);\n"

		"vec2 toCenter = pos - o_tex_coord;\n"
		"vec3 normal = vec3(toCenter, 1.0);\n"
		"normal = normalize(normal);\n"
		"vec3 normalColor = 0.5 * (normal + 1.0);\n"
		"normal_channel= vec4(normalColor, 1.0);\n"

		"vec2 dir = normalize(pos - light_norm);\n"
		"float normal_fall_off = clamp(dot(dir, normalColor.xy), 0.0, 1);\n"

		"color_channel = o_color * intensity * radial_fall_off * angular_fall_off * normal_fall_off;\n"
	"}\n";

const char* light_fragment_src =
	"#version 440 core\n"
	"layout (location = 0) out vec4 color;\n"

	"in vec4 o_color;\n"
	"in vec2 o_tex_coord;\n"
	"in float o_tex_id;\n"

	"uniform vec2 light_pos;\n"
	"uniform vec2 dim;\n"

	"float intensity = 0.9;\n"
	"vec2 light_norm = light_pos / dim;\n"
	"vec2 pix_size = vec2(2);\n"

	"void main() {\n"
	"vec2 block_coord = floor(gl_FragCoord.xy / pix_size) * pix_size;\n"
	"vec2 uv = block_coord / dim;\n"

	"float radial_fall_off = pow(1 - distance(light_norm, uv.xy), 2);\n"
	"float angle = abs(atan(uv.y - light_norm.y, uv.x - light_norm.x));\n"
	"float angular_fall_off = smoothstep(3.14 / 2, 0, angle);\n"

	"color = o_color * intensity * radial_fall_off * angular_fall_off;\n"
	"}\n";

const char* mix_fragment_src =
	"#version 440 core\n"
	"layout (location = 0) out vec4 color;\n"

	"in vec4 o_color;\n"
	"in vec2 o_tex_coord;\n"
	"in float o_tex_id;\n"

	"uniform sampler2D color_texture;\n"
	"uniform sampler2D light_texture;\n"

	"void main() {\n"
	"vec4 colo_1 = texture(color_texture, o_tex_coord);\n"
	"vec4 colo_2 = texture(light_texture, o_tex_coord);\n"
	"color = colo_1 + colo_2 * 0.3;\n"
	"}\n";

typedef struct {
	u32 fbo;
	u32 color_texture;
	u32 normal_texture;
} FBO;

FBO generate_fbo(u32 width, u32 height) {
	u32 framebuffer, color_texture, normal_texture;

	// Generate and bind framebuffer
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// Create and attach color texture
	glGenTextures(1, &color_texture);
	glBindTexture(GL_TEXTURE_2D, color_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);

	// Create and attach normal texture
	glGenTextures(1, &normal_texture);
	glBindTexture(GL_TEXTURE_2D, normal_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal_texture, 0);

	GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	GLCall(glDrawBuffers(2, attachments));

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebuffer is not complete!\n");
	}

	return (FBO) {
		framebuffer,
		color_texture,
		normal_texture
	};
}

int main(int argc, char** argv) {
	// Window
	Window window = Result_Window_unwrap(
		window_new("Light", WIN_WIDTH, WIN_HEIGHT)
	);

	// Creating custom shader
	Shader color_shader = Result_Shader_unwrap(shader_new(vertex_src, fragment_src));
	Shader light_shader = Result_Shader_unwrap(shader_new(vertex_src, light_fragment_src));
	Shader mix_shader = Result_Shader_unwrap(shader_new(vertex_src, mix_fragment_src));

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

	FBO light_fbo = generate_fbo(SURF_WIDTH, SURF_HEIGHT);
	FBO colo_fbo = generate_fbo(SURF_WIDTH, SURF_HEIGHT);
	FBO mix_fbo = generate_fbo(SURF_WIDTH, SURF_HEIGHT);

	// Lights
	v2 light_pos = {
		SURF_WIDTH / 2 - 40, SURF_HEIGHT / 2
	};
	v2 light_size = { WIN_WIDTH, WIN_HEIGHT};

	while (!window.should_close) {

		// Color pass
		{
			imr_switch_shader(&imr, color_shader);

			glViewport(0, 0, SURF_WIDTH, SURF_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, colo_fbo.fbo);

			imr_clear((v4) { 0, 0, 0, 1 });

			imr_begin(&imr);

			m4 mvp = ocamera_calc_mvp(&cam);
			imr_update_mvp(&imr, mvp);

			int loc = GLCall(glGetUniformLocation(color_shader, "light_pos"));
			assert(loc != -1, "Cannot find uniform: light_pos\n");
			GLCall(glUniform2f(loc, light_pos.x, light_pos.y));

			loc = GLCall(glGetUniformLocation(color_shader, "dim"));
			assert(loc != -1, "Cannot find uniform: dim\n");
			GLCall(glUniform2f(loc, SURF_WIDTH, SURF_HEIGHT));

			v2 size = { 20, 20 };
			imr_push_quad(
				&imr,
				(v3) {
					SURF_WIDTH / 2 - size.x / 2,
					SURF_HEIGHT / 2 - size.y / 2,
					0
				},
				size,
				rotate_z(0),
				(v4) { 0.3, 0.5, 0.5, 1 }
			);

			imr_end(&imr);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		// Light pass
		{
			imr_switch_shader(&imr, light_shader);

			glViewport(0, 0, SURF_WIDTH, SURF_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, light_fbo.fbo);

			imr_clear((v4) { 0, 0, 0, 1 });

			imr_begin(&imr);

			m4 mvp = ocamera_calc_mvp(&cam);
			imr_update_mvp(&imr, mvp);

			int loc = GLCall(glGetUniformLocation(light_shader, "light_pos"));
			assert(loc != -1, "Cannot find uniform: light_pos\n");
			GLCall(glUniform2f(loc, light_pos.x, light_pos.y));

			loc = GLCall(glGetUniformLocation(light_shader, "dim"));
			assert(loc != -1, "Cannot find uniform: dim\n");
			GLCall(glUniform2f(loc, SURF_WIDTH, SURF_HEIGHT));

			imr_push_quad(
				&imr,
				(v3) {
					light_pos.x - light_size.x / 2,
					light_pos.y - light_size.y / 2,
					0
				},
				light_size,
				rotate_z(0),
				(v4) { 1, 1, 1, 1 }
			);

			imr_end(&imr);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		//// Combining pass
		{
			imr_switch_shader(&imr, mix_shader);

			glViewport(0, 0, SURF_WIDTH, SURF_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, mix_fbo.fbo);

			GLCall(glBindTextureUnit(colo_fbo.color_texture, colo_fbo.color_texture));
			GLCall(glBindTextureUnit(light_fbo.color_texture, light_fbo.color_texture));

			imr_clear((v4) { 0, 0, 0, 1 });

			imr_begin(&imr);

			m4 mvp = ocamera_calc_mvp(&cam);
			imr_update_mvp(&imr, mvp);

			int loc = GLCall(glGetUniformLocation(mix_shader, "color_texture"));
			assert(loc != -1, "Cannot find uniform: color_texture\n");
			GLCall(glUniform1i(loc, colo_fbo.color_texture));

			loc = GLCall(glGetUniformLocation(mix_shader, "light_texture"));
			assert(loc != -1, "Cannot find uniform: light_texture\n");
			GLCall(glUniform1i(loc, light_fbo.color_texture));

			imr_push_quad(
				&imr,
				(v3) { 0, 0, 0 },
				(v2) { SURF_WIDTH, SURF_HEIGHT },
				rotate_z(0),
				(v4) { 1, 1, 1, 1 }
			);

			imr_end(&imr);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		// Final render
		{
			u32 texture_to_render = mix_fbo.color_texture;

			imr_switch_shader_to_default(&imr);

			glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
			GLCall(glBindTextureUnit(texture_to_render, texture_to_render));

			imr_clear((v4) { 0, 0, 0, 1 });
			
			imr_begin(&imr);

			m4 mvp = ocamera_calc_mvp(&cam2);
			imr_update_mvp(&imr, mvp);

			imr_push_quad_tex(
				&imr,
				(v3) { 0, 0, 0 },
				(v2) { 800, 600},
				(Rect) { 0, 0, 1, 1 },
				texture_to_render,
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

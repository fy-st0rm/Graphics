#include <stdio.h>

#include "window/window.h"
#include "graphics/imr.h"
#include "camera/camera.h"
#include "math/utils.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define SURF_WIDTH 133
#define SURF_HEIGHT 100

#define SHADER_SRC(...)\
	"#version 440 core\n"\
	"#define PI 3.1415926538\n"\
	#__VA_ARGS__\

const char* vertex_src = SHADER_SRC(
	layout (location = 0) in vec3 position;
	layout (location = 1) in vec4 color;
	layout (location = 2) in vec2 tex_coord;
	layout (location = 3) in float tex_id;

	uniform mat4 mvp;

	out vec4 o_color;
	out vec2 o_tex_coord;
	out float o_tex_id;

	void main() {
		o_color = color;
		o_tex_coord = tex_coord;
		o_tex_id = tex_id;
		gl_Position = mvp * vec4(position, 1.0f);
	}
);

const char* fragment_src = SHADER_SRC(
	layout (location = 0) out vec4 color_channel;
	layout (location = 1) out vec4 normal_channel;

	in vec4 o_color;
	in vec2 o_tex_coord;
	in float o_tex_id;

	uniform vec2 light_pos;
	uniform vec2 dim;

	vec2 pos = vec2(133 / 2, 100 / 2) / dim;

	float intensity = 0.9;
	vec2 light_norm = light_pos / dim;
	vec2 pix_size = vec2(2);

	void main() {
		vec2 block_coord = floor(gl_FragCoord.xy / pix_size) * pix_size;
		vec2 uv = block_coord / dim;

		float radial_fall_off = pow(1 - distance(light_norm, uv.xy), 2);
		float angle = abs(atan(uv.y - light_norm.y, uv.x - light_norm.x));
		float angular_fall_off = smoothstep(PI / 2, 0, angle);

		vec2 toCenter = pos - o_tex_coord;
		vec3 normal = vec3(toCenter, 1.0);
		normal = normalize(normal);
		vec3 normalColor = 0.5 * (normal + 1.0);
		normal_channel= vec4(normalColor, 1.0);

		vec2 dir = normalize(pos - light_norm);
		float normal_fall_off = clamp(dot(dir, normalColor.xy), 0.0, 1);

		color_channel = o_color
		* intensity
		* radial_fall_off
		* angular_fall_off
		* normal_fall_off;
	}
);

const char* light_fragment_src = SHADER_SRC(
	layout (location = 0) out vec4 color;
	
	in vec4 o_color;
	in vec2 o_tex_coord;
	in float o_tex_id;

	uniform vec2 dim;

	uniform vec2 light_pos;
	uniform float intensity;
	uniform float dir;
	uniform float fov;

	vec2 light_norm = light_pos / dim;
	vec2 pix_size = vec2(2);

	vec2 rotate(vec2 v, float angle) {
		float cosAngle = cos(angle);
		float sinAngle = sin(angle);
		return vec2(
			v.x * cosAngle - v.y * sinAngle,
			v.x * sinAngle + v.y * cosAngle
		);
	}

	void main() {
		vec2 block_coord = floor(gl_FragCoord.xy / pix_size) * pix_size;
		vec2 uv = block_coord / dim;

		// Calculate the vector from the light to the current fragment
		vec2 toFragment = uv - light_norm;
		
		// Rotate this vector around the light position
		vec2 rotatedToFragment = rotate(toFragment, dir);
		
		// Adjust the UV coordinates accordingly
		uv = rotatedToFragment + light_norm;

		float radial_fall_off = pow(1 - distance(light_norm, uv.xy), 2);
		float angle = abs(atan(uv.y - light_norm.y, uv.x - light_norm.x));

		float angular_fall_off = smoothstep(fov, 0, angle);

		color = o_color * intensity * radial_fall_off * angular_fall_off;
	}
);

const char* mix_fragment_src = SHADER_SRC(
	layout (location = 0) out vec4 color;

	in vec4 o_color;
	in vec2 o_tex_coord;
	in float o_tex_id;

	uniform sampler2D color_texture;
	uniform sampler2D light_texture;

	void main() {
	vec4 colo_1 = texture(color_texture, o_tex_coord);
	vec4 colo_2 = texture(light_texture, o_tex_coord);
	color = colo_1 + colo_2 * 0.3;
	}
);

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

typedef struct {
	v2 pos;
	f32 intensity;
	f32 fov;
	f32 dir;
} Light;

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

	Light light = {
		.pos = {
			SURF_WIDTH / 2 - 40, SURF_HEIGHT / 2
		},
		.intensity = 0.9f,
		.fov = PI / 2,
		.dir = 0
	};

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

			int loc = GLCall(glGetUniformLocation(light_shader, "dim"));
			assert(loc != -1, "Cannot find uniform: dim\n");
			GLCall(glUniform2f(loc, SURF_WIDTH, SURF_HEIGHT));

			loc = GLCall(glGetUniformLocation(light_shader, "light_pos"));
			assert(loc != -1, "Cannot find uniform: light_pos\n");
			GLCall(glUniform2f(loc, light.pos.x, light.pos.y));

			loc = GLCall(glGetUniformLocation(light_shader, "intensity"));
			assert(loc != -1, "Cannot find uniform: intensity\n");
			GLCall(glUniform1f(loc, light.intensity));

			loc = GLCall(glGetUniformLocation(light_shader, "dir"));
			assert(loc != -1, "Cannot find uniform: dir\n");
			GLCall(glUniform1f(loc, light.dir));

			loc = GLCall(glGetUniformLocation(light_shader, "fov"));
			assert(loc != -1, "Cannot find uniform: fov\n");
			GLCall(glUniform1f(loc, light.fov));

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
			u32 texture_to_render = light_fbo.color_texture;

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

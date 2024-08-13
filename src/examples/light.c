#include <stdio.h>

#include "window/window.h"
#include "graphics/imr.h"
#include "camera/camera.h"
#include "math/utils.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define SURF_WIDTH 133
#define SURF_HEIGHT 100

#define TOTAL_LIGHTS 4

#define SHADER_SRC(...)\
	"#version 440 core\n"\
	"#define PI 3.1415926538\n"\
	"#define TOTAL_LIGHTS 4\n"\
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
	out mat4 o_mvp;

	void main() {
		o_color = color;
		o_tex_coord = tex_coord;
		o_tex_id = tex_id;
		o_mvp = mvp;
		gl_Position = mvp * vec4(position, 1.0f);
	}
);

const char* fragment_src = SHADER_SRC(
	layout (location = 0) out vec4 color_channel;
	layout (location = 1) out vec4 normal_channel;

	in vec4 o_color;
	in vec2 o_tex_coord;
	in float o_tex_id;
	in mat4 o_mvp;

	struct Light {
		vec2 pos;
		float radius;
		float intensity;
		float dir;
		float fov;
		vec4 color;
	};

	uniform vec2 dim;
	uniform Light light[TOTAL_LIGHTS];

	vec2 pos = vec2(133 / 2, 100 / 2) / dim;
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
		vec4 final_color = vec4(0,0,0,1);

		for (int i = 0; i < TOTAL_LIGHTS; i++) {
			// Calculating pixelated uv
			vec2 block_coord = floor(gl_FragCoord.xy / pix_size) * pix_size;
			vec2 uv = (block_coord - 0.5 * dim) / dim * 2.0;

			// Transforming light with camera
			vec4 nl = vec4(light[i].pos, 0, 1);
			vec4 tl = o_mvp * nl;
			vec2 light_norm = tl.xy / tl.w;

			// Rotating the uv with light direction
			vec2 toFragment = uv - light_norm;
			vec2 rotatedToFragment = rotate(toFragment, light[i].dir);
			uv = rotatedToFragment + light_norm;

			// Calculating fall ofs
			float dist = length(toFragment);
			float radial_fall_off = pow(clamp(1.0 - dist / light[i].radius, 0.0, 1.0), 2);
			float angle = abs(atan(uv.y - light_norm.y, uv.x - light_norm.x));
			float angular_fall_off = smoothstep(light[i].fov, 0, angle);

			//vec2 toCenter = pos - o_tex_coord;
			//vec3 normal = vec3(toCenter, 1.0);
			//normal = normalize(normal);
			//vec3 normalColor = 0.5 * (normal + 1.0);
			//normal_channel= vec4(normalColor, 1.0);
			//vec2 dir = normalize(pos - light_norm);
			//float normal_fall_off = clamp(dot(dir, normalColor.xy), 0.0, 1);

			final_color += o_color
			* light[i].intensity
			* light[i].color
			* radial_fall_off
			* angular_fall_off;
		}
		color_channel = final_color;
	}
);

const char* light_vertex_src = SHADER_SRC(
	layout (location = 0) in vec3 position;
	layout (location = 1) in vec4 color;
	layout (location = 2) in vec2 tex_coord;
	layout (location = 3) in float tex_id;

	uniform mat4 mvp;

	out vec4 o_color;
	out vec2 o_tex_coord;
	out float o_tex_id;
	out mat4 o_mvp;

	void main() {
		o_color = color;
		o_tex_coord = tex_coord;
		o_tex_id = tex_id;
		o_mvp = mvp;
		gl_Position = vec4(position, 1.0f);
	}
);

const char* light_fragment_src = SHADER_SRC(
	layout (location = 0) out vec4 color;
	
	in vec4 o_color;
	in vec2 o_tex_coord;
	in float o_tex_id;
	in mat4 o_mvp;

	struct Light {
		vec2 pos;
		float radius;
		float intensity;
		float dir;
		float fov;
		vec4 color;
	};

	uniform vec2 dim;
	uniform Light light[TOTAL_LIGHTS];

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
		vec4 final_color = vec4(0, 0, 0, 1);
		for (int i = 0; i < TOTAL_LIGHTS; i++) {
			// Calculating pixelated uv
			vec2 block_coord = floor(gl_FragCoord.xy / pix_size) * pix_size;
			vec2 uv = (block_coord - 0.5 * dim) / dim * 2.0;

			// Transforming light with camera
			vec4 nl = vec4(light[i].pos, 0, 1);
			vec4 tl = o_mvp * nl;
			vec2 light_norm = tl.xy / tl.w;

			// Rotating the uv with light direction
			vec2 toFragment = uv - light_norm;
			vec2 rotatedToFragment = rotate(toFragment, light[i].dir);
			uv = rotatedToFragment + light_norm;

			// Calculating fall ofs
			float dist = length(toFragment);
			float radial_fall_off = pow(clamp(1.0 - dist / light[i].radius, 0.0, 1.0), 2);
			float angle = abs(atan(uv.y - light_norm.y, uv.x - light_norm.x));
			float angular_fall_off = smoothstep(light[i].fov, 0, angle);
			
			final_color += o_color
			* radial_fall_off
			* angular_fall_off
			* light[i].intensity
			* light[i].color;
		}

		color = final_color;
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
	color = colo_1 + colo_2 * 0.5;
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
	f32 radius;
	f32 fov;
	f32 dir;
	v4 color;
} Light;

int main(int argc, char** argv) {
	// Window
	Window window = unwrap(window_new("Light", WIN_WIDTH, WIN_HEIGHT));

	// Creating custom shader
	Shader color_shader = unwrap(shader_new(vertex_src, fragment_src));
	Shader light_shader = unwrap(shader_new(light_vertex_src, light_fragment_src));
	Shader mix_shader = unwrap(shader_new(light_vertex_src, mix_fragment_src));

	// Renderer
	IMR imr = unwrap(imr_new());

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
	Light light_1 = {
		.pos = {
			SURF_WIDTH / 2 + 40, SURF_HEIGHT / 2 + 20
		},
		.radius = 1.0f,
		.intensity = 0.9f,
		.fov = PI / 2,
		.dir = to_radians(135),
		.color = { 0.8, 0.5, 0, 1 }
	};

	Light light_2 = {
		.pos = {
			SURF_WIDTH / 2 - 40, SURF_HEIGHT / 2 + 20
		},
		.radius = 1.0f,
		.intensity = 0.9f,
		.fov = PI / 2,
		.dir = to_radians(45),
		.color = { 0.0, 0.5, 0.8, 1 }
	};

	Light light_3 = {
		.pos = {
			SURF_WIDTH / 2 - 40, SURF_HEIGHT / 2 - 20
		},
		.radius = 1.0f,
		.intensity = 0.9f,
		.fov = PI / 2,
		.dir = to_radians(315),
		.color = { 0.8, 0.0, 0.8, 1 }
	};

	Light light_4 = {
		.pos = {
			SURF_WIDTH / 2 + 40, SURF_HEIGHT / 2 - 20
		},
		.radius = 1.0f,
		.intensity = 0.9f,
		.fov = PI / 2,
		.dir = to_radians(205),
		.color = { 0.8, 0.0, 0.0, 1 }
	};

	Light lights[TOTAL_LIGHTS] = { light_1, light_2, light_3, light_4 };

	while (!window.should_close) {

		// Event
		{
			Event event;
			while(event_poll(window, &event)) {
				if (event.type == KEYDOWN) {
					switch (event.e.key) {
						case GLFW_KEY_EQUAL:
							ocamera_change_zoom(&cam, 0.1);
							break;
						case GLFW_KEY_MINUS:
							ocamera_change_zoom(&cam, -0.1);
							break;

						case GLFW_KEY_W:
							ocamera_change_pos(&cam, (v2) { 0, 0.1 });
							break;
						case GLFW_KEY_A:
							ocamera_change_pos(&cam, (v2) { -0.1, 0 });
							break;
						case GLFW_KEY_S:
							ocamera_change_pos(&cam, (v2) { 0, -0.1 });
							break;
						case GLFW_KEY_D:
							ocamera_change_pos(&cam, (v2) { 0.1, 0 });
							break;
					}
				}
			}
		}

		// Color pass
		{
			imr_switch_shader(&imr, color_shader);

			glViewport(0, 0, SURF_WIDTH, SURF_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, colo_fbo.fbo);

			imr_clear((v4) { 0, 0, 0, 1 });

			imr_begin(&imr);

			m4 mvp = ocamera_calc_mvp(&cam);
			imr_update_mvp(&imr, mvp);

			int loc = GLCall(glGetUniformLocation(color_shader, "dim"));
			assert(loc != -1, "Cannot find uniform: dim\n");
			GLCall(glUniform2f(loc, SURF_WIDTH, SURF_HEIGHT));

			for (int i = 0; i < TOTAL_LIGHTS; i++) {
				char* uni_name = "light[%d].%s";
				char buff[100];

				sprintf(buff, uni_name, i, "pos");
				loc = GLCall(glGetUniformLocation(color_shader, buff));
				assert(loc != -1, "Cannot find uniform: %s\n", buff);
				GLCall(glUniform2f(loc, lights[i].pos.x, lights[i].pos.y));

				sprintf(buff, uni_name, i, "radius");
				loc = GLCall(glGetUniformLocation(color_shader, buff));
				assert(loc != -1, "Cannot find uniform: %s\n", buff);
				GLCall(glUniform1f(loc, lights[i].radius));

				sprintf(buff, uni_name, i, "intensity");
				loc = GLCall(glGetUniformLocation(color_shader, buff));
				assert(loc != -1, "Cannot find uniform: light.intensity\n");
				GLCall(glUniform1f(loc, lights[i].intensity));

				sprintf(buff, uni_name, i, "dir");
				loc = GLCall(glGetUniformLocation(color_shader, buff));
				assert(loc != -1, "Cannot find uniform: light.dir\n");
				GLCall(glUniform1f(loc, lights[i].dir));

				sprintf(buff, uni_name, i, "fov");
				loc = GLCall(glGetUniformLocation(color_shader, buff));
				assert(loc != -1, "Cannot find uniform: light.fov\n");
				GLCall(glUniform1f(loc, lights[i].fov));

				sprintf(buff, uni_name, i, "color");
				loc = GLCall(glGetUniformLocation(color_shader, buff));
				assert(loc != -1, "Cannot find uniform: light.color\n");
				GLCall(glUniform4f(loc, lights[i].color.r, lights[i].color.g, lights[i].color.b, lights[i].color.a));
			}

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

			int loc;
			loc = GLCall(glGetUniformLocation(light_shader, "dim"));
			assert(loc != -1, "Cannot find uniform: dim\n");
			GLCall(glUniform2f(loc, SURF_WIDTH, SURF_HEIGHT));

			for (int i = 0; i < TOTAL_LIGHTS; i++) {
				char* uni_name = "light[%d].%s";
				char buff[100];

				sprintf(buff, uni_name, i, "pos");
				loc = GLCall(glGetUniformLocation(light_shader, buff));
				assert(loc != -1, "Cannot find uniform: %s\n", buff);
				GLCall(glUniform2f(loc, lights[i].pos.x, lights[i].pos.y));

				sprintf(buff, uni_name, i, "radius");
				loc = GLCall(glGetUniformLocation(light_shader, buff));
				assert(loc != -1, "Cannot find uniform: %s\n", buff);
				GLCall(glUniform1f(loc, lights[i].radius));

				sprintf(buff, uni_name, i, "intensity");
				loc = GLCall(glGetUniformLocation(light_shader, buff));
				assert(loc != -1, "Cannot find uniform: %s\n", buff);
				GLCall(glUniform1f(loc, lights[i].intensity));

				sprintf(buff, uni_name, i, "dir");
				loc = GLCall(glGetUniformLocation(light_shader, buff));
				assert(loc != -1, "Cannot find uniform: %s\n", buff);
				GLCall(glUniform1f(loc, lights[i].dir));

				sprintf(buff, uni_name, i, "fov");
				loc = GLCall(glGetUniformLocation(light_shader, buff));
				assert(loc != -1, "Cannot find uniform: %s\n", buff);
				GLCall(glUniform1f(loc, lights[i].fov));

				sprintf(buff, uni_name, i, "color");
				loc = GLCall(glGetUniformLocation(light_shader, buff));
				assert(loc != -1, "Cannot find uniform: %s\n", buff);
				GLCall(glUniform4f(loc, lights[i].color.r, lights[i].color.g, lights[i].color.b, lights[i].color.a));
			}

			imr_push_quad(
				&imr,
				(v3) { -1, -1, 0 },
				(v2) { 2, 2 },
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

			// m4 mvp = ocamera_calc_mvp(&cam);
			// imr_update_mvp(&imr, mvp);

			int loc = GLCall(glGetUniformLocation(mix_shader, "color_texture"));
			assert(loc != -1, "Cannot find uniform: color_texture\n");
			GLCall(glUniform1i(loc, colo_fbo.color_texture));

			loc = GLCall(glGetUniformLocation(mix_shader, "light_texture"));
			assert(loc != -1, "Cannot find uniform: light_texture\n");
			GLCall(glUniform1i(loc, light_fbo.color_texture));

			imr_push_quad(
				&imr,
				(v3) { -1, -1, 0 },
				(v2) { 2, 2},
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

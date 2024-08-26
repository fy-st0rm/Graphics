#ifndef __SHADER_SRC_H__
#define __SHADER_SRC_H__

// TODO: Merge color_frag shader and light_frag shader into one using two export textures

#define SHADER_SRC(...)\
	"#version 440 core\n"\
	"#define PI 3.1415926538\n"\
	"#define MAX_LIGHT_CAP 100\n"\
	"vec2 pix_size = vec2(1);\n"\
	#__VA_ARGS__\


static const char* color_vertex_src = SHADER_SRC(
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

static const char* color_fragment_src = SHADER_SRC(
	layout (location = 0) out vec4 color_channel;

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
	uniform Light light[MAX_LIGHT_CAP];
	uniform int light_cnt;

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
		vec4 final_light_color = vec4(0,0,0,1);

		for (int i = 0; i < light_cnt; i++) {

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
			* light[i].color
			* light[i].intensity
			* radial_fall_off
			* angular_fall_off;
		}

	// TODO: Add proper ambient light
		color_channel = final_color + o_color * vec4(0.1);
	}
);

static const char* light_vertex_src = SHADER_SRC(
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

static const char* light_fragment_src = SHADER_SRC(
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
	uniform Light light[MAX_LIGHT_CAP];
	uniform int light_cnt;

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

		for (int i = 0; i < light_cnt; i++) {

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

static const char* mix_fragment_src = SHADER_SRC(
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


#endif // __SHADER_SRC_H__

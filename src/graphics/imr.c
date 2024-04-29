#include "imr.h"

DEFINE_RESULT(IMR, IMR);

const char* v_src =
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

const char* f_src =
	"#version 440 core\n"
	"layout (location = 0) out vec4 color;\n"
	"uniform sampler2D textures[32];\n"
	"in vec4 o_color;\n"
	"in vec2 o_tex_coord;\n"
	"in float o_tex_id;\n"
	"void main() {\n"
	"int index = int(o_tex_id);\n"
	"color = texture(textures[index], o_tex_coord) * o_color;\n"
//	"color = o_color;\n"
	"}\n";

Result_IMR imr_new() {
	u32 vao, vbo;
	Shader shader;

	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	// Buffers
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao));

	GLCall(glGenBuffers(1, &vbo));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GLCall(glBufferData(GL_ARRAY_BUFFER, MAX_VBO_SIZE, NULL, GL_DYNAMIC_DRAW));

	// VAO format
	STATIC_ASSERT(
		10 == sizeof(Vertex) / sizeof(f32),
		"Vertex has been updated. Update VAO format."
	);

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*) offsetof(Vertex, pos)));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*) offsetof(Vertex, color)));
	GLCall(glEnableVertexAttribArray(2));
	GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*) offsetof(Vertex, tex_coord)));
	GLCall(glEnableVertexAttribArray(3));
	GLCall(glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*) offsetof(Vertex, tex_id)));

	// Generating white texture
	u32 data = 0xffffffff;
	Texture white = Result_Texture_unwrap(texture_from_data(1, 1, &data));
	texture_bind(white);

	// Shader
	Result_Shader rs = shader_new(v_src, f_src);
	if (rs.status == ERROR) {
		return Result_IMR_err(Result_Shader_unwrap_err(rs));
	}

	shader = Result_Shader_unwrap(rs);
	GLCall(glUseProgram(shader));

	// Providing texture samples
	u32 samplers[TEXTURE_SAMPLE_AMT];
	for (u32 i = 0; i < TEXTURE_SAMPLE_AMT; i++)
		samplers[i] = i;
	
	// Providing samplers to the shader
	int loc = GLCall(glGetUniformLocation(shader, "textures"));
	assert(loc != -1, "Cannot find uniform: textures\n");
	GLCall(glUniform1iv(loc, TEXTURE_SAMPLE_AMT, samplers));

	return Result_IMR_ok((IMR) {
		.vao = vao,
		.vbo = vbo,
		.shader = shader,
		.buff_idx = 0,
		.white = white
	});
}

void imr_delete(IMR* imr) {
	GLCall(glDeleteVertexArrays(1, &imr->vao));
	GLCall(glDeleteBuffers(1, &imr->vbo));
	texture_delete(imr->white);
	shader_delete(imr->shader);
}

void imr_begin(IMR* imr) {
	imr->buff_idx = 0;
	texture_bind(imr->white);
	GLCall(glUseProgram(imr->shader));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, imr->vbo));
}

void imr_end(IMR* imr) {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, imr->vbo));
	GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(imr->buffer), imr->buffer));

	GLCall(glBindVertexArray(imr->vao));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, imr->buff_idx / VERTEX_SIZE));
}

void imr_update_mvp(IMR* imr, m4 mvp) {
	i32 loc = GLCall(glGetUniformLocation(imr->shader, "mvp"));
	GLCall(glUniformMatrix4fv(loc, 1, GL_TRUE, &mvp.m[0][0]));
}

void imr_push_vertex(IMR* imr, Vertex v) {
	STATIC_ASSERT(
		10 == sizeof(Vertex) / sizeof(f32),
		"Vertex has been updated. Update this method."
	);

	imr->buffer[imr->buff_idx++] = v.pos.x;
	imr->buffer[imr->buff_idx++] = v.pos.y;
	imr->buffer[imr->buff_idx++] = v.pos.z;
	imr->buffer[imr->buff_idx++] = v.color.r;
	imr->buffer[imr->buff_idx++] = v.color.g;
	imr->buffer[imr->buff_idx++] = v.color.b;
	imr->buffer[imr->buff_idx++] = v.color.a;
	imr->buffer[imr->buff_idx++] = v.tex_coord.x;
	imr->buffer[imr->buff_idx++] = v.tex_coord.y;
	imr->buffer[imr->buff_idx++] = v.tex_id;
}

void imr_push_quad(IMR* imr, v2 pos, v2 size, Rect tex_rect, f32 tex_id, m4 rot, v4 color) {
	if (((imr->buff_idx + 6 * VERTEX_SIZE) / VERTEX_SIZE) >= MAX_VERT_CNT) {
		imr_end(imr);
		imr_begin(imr);
	}

	Vertex p1, p2, p3, p4, p5, p6;

	// Rotating over origin
	p1.pos = m4_mul_v3(rot, (v3) { -size.x / 2, -size.y / 2, 0.0f });
	p2.pos = m4_mul_v3(rot, (v3) {  size.x / 2, -size.y / 2, 0.0f });
	p3.pos = m4_mul_v3(rot, (v3) {  size.x / 2,  size.y / 2, 0.0f });

	p4.pos = m4_mul_v3(rot, (v3) {  size.x / 2,  size.y / 2, 0.0f });
	p5.pos = m4_mul_v3(rot, (v3) { -size.x / 2,  size.y / 2, 0.0f });
	p6.pos = m4_mul_v3(rot, (v3) { -size.x / 2, -size.y / 2, 0.0f });

	// Shifting to the desired position
	p1.pos = v3_add(p1.pos, (v3) { pos.x + size.x / 2, pos.y + size.y / 2, 0.0f });
	p2.pos = v3_add(p2.pos, (v3) { pos.x + size.x / 2, pos.y + size.y / 2, 0.0f });
	p3.pos = v3_add(p3.pos, (v3) { pos.x + size.x / 2, pos.y + size.y / 2, 0.0f });

	p4.pos = v3_add(p4.pos, (v3) { pos.x + size.x / 2, pos.y + size.y / 2, 0.0f });
	p5.pos = v3_add(p5.pos, (v3) { pos.x + size.x / 2, pos.y + size.y / 2, 0.0f });
	p6.pos = v3_add(p6.pos, (v3) { pos.x + size.x / 2, pos.y + size.y / 2, 0.0f });

	// Making the texure coordinates
	p1.tex_coord = (v2) { tex_rect.x, tex_rect.y };
	p2.tex_coord = (v2) { tex_rect.x + tex_rect.w, tex_rect.y };
	p3.tex_coord = (v2) { tex_rect.x + tex_rect.w, tex_rect.y + tex_rect.h };
	p4.tex_coord = (v2) { tex_rect.x + tex_rect.w, tex_rect.y + tex_rect.h };
	p5.tex_coord = (v2) { tex_rect.x, tex_rect.y + tex_rect.h };
	p6.tex_coord = (v2) { tex_rect.x, tex_rect.y };

	p1.color = p2.color = p3.color = p4.color = p5.color = p6.color = color;
	p1.tex_id = p2.tex_id = p3.tex_id = p4.tex_id = p5.tex_id = p6.tex_id = tex_id;

	imr_push_vertex(imr, p1);
	imr_push_vertex(imr, p2);
	imr_push_vertex(imr, p3);
	imr_push_vertex(imr, p4);
	imr_push_vertex(imr, p5);
	imr_push_vertex(imr, p6);
}

void imr_push_triangle(IMR* imr, v2 p1, v2 p2, v2 p3, m4 rot, v4 color) {
	assert(0, "Havent implemented texture support yet.");
	if (((imr->buff_idx + 3 * VERTEX_SIZE) / VERTEX_SIZE) >= MAX_VERT_CNT) {
		imr_end(imr);
		imr_begin(imr);
	}

	v2 centroid = {
		(p1.x + p2.x + p3.x) / 3.0f,
		(p1.y + p2.y + p3.y) / 3.0f
	};

	Vertex a1, a2, a3;

	// Rotating over origin
	a1.pos = m4_mul_v3(rot, (v3) { p1.x - centroid.x, p1.y - centroid.y, 0.0f });
	a2.pos = m4_mul_v3(rot, (v3) { p2.x - centroid.x, p2.y - centroid.y, 0.0f });
	a3.pos = m4_mul_v3(rot, (v3) { p3.x - centroid.x, p3.y - centroid.y, 0.0f });

	// Shifting to the desired position
	a1.pos = v3_add(a1.pos, (v3) { centroid.x, centroid.y, 0.0f });
	a2.pos = v3_add(a2.pos, (v3) { centroid.x, centroid.y, 0.0f });
	a3.pos = v3_add(a3.pos, (v3) { centroid.x, centroid.y, 0.0f });

	a1.color = a2.color = a3.color = color;

	imr_push_vertex(imr, a1);
	imr_push_vertex(imr, a2);
	imr_push_vertex(imr, a3);
}

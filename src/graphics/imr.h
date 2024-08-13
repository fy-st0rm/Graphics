#ifndef __IMR_H__
#define __IMR_H__

#include "core/defines.h"
#include "core/log.h"
#include "core/result.h"
#include "math/vec.h"
#include "math/mat.h"
#include "math/rect.h"
#include "shader.h"
#include "texture.h"

typedef struct {
	v3 pos;
	v4 color;
	v2 tex_coord;
	f32 tex_id;
} Vertex;

typedef struct {
	v3 a, b, c;
} Triangle;

#define TEXTURE_SAMPLE_AMT 32
#define VERTEX_SIZE   10
#define MAX_VERT_CNT  10000
#define MAX_BUFF_CAP  MAX_VERT_CNT  * VERTEX_SIZE
#define MAX_VBO_SIZE  MAX_BUFF_CAP  * sizeof(f32)

STATIC_ASSERT(VERTEX_SIZE == sizeof(Vertex) / sizeof(f32), "Size of vertex missmatched");

typedef struct {
	u32 vao, vbo;
	Shader shader;
	Shader def_shader;
	f32 buffer[MAX_BUFF_CAP];
	u32 buff_idx;
	Texture white;
} IMR;

RESULT(IMR, IMR);

Result_IMR imr_new();
void imr_delete(IMR* imr);
void imr_clear(v4 color);
void imr_begin(IMR* imr);
void imr_end(IMR* imr);
void imr_switch_shader(IMR* imr, Shader shader);
void imr_switch_shader_to_default(IMR* imr);
void imr_update_mvp(IMR* imr, m4 mvp);
void imr_push_vertex(IMR* imr, Vertex v);
void imr_push_quad(IMR* imr, v3 pos, v2 size, m4 rot, v4 color);
void imr_push_quad_tex(IMR* imr, v3 pos, v2 size, Rect tex_rect, f32 tex_id, m4 rot, v4 color);
void imr_push_triangle(IMR* imr, v3 p1, v3 p2, v3 p3, m4 rot, v4 color);
void imr_push_triangle_tex(IMR* imr, v3 p1, v3 p2, v3 p3, Triangle tex_coord, f32 tex_id, m4 rot, v4 color);

#endif // __IMR_H__

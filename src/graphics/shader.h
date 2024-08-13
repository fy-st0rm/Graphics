#ifndef __SHADER_H__
#define __SHADER_H__

#include "GL/glew.h"
#include "core/log.h"
#include "core/defines.h"
#include "core/result.h"

typedef u32 Shader;
typedef enum {
	VERTEX_SHADER = GL_VERTEX_SHADER,
	FRAGMENT_SHADER = GL_FRAGMENT_SHADER
} Shader_Type;

RESULT(Shader, Shader);

Result_Shader shader_new(const char* v_src, const char* f_src);
void shader_delete(Shader id);
Result_u32 shader_compile(Shader_Type type, const char* shader_src);

#endif // __SHADER_H__

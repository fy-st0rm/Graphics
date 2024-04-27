#include "shader.h"

DEFINE_RESULT(Shader, Shader);

Result_Shader shader_new(const char* v_src, const char* f_src) {
	u32 program = glCreateProgram();

	Result_u32 rvs = shader_compile(GL_VERTEX_SHADER,   v_src);
	Result_u32 rfs = shader_compile(GL_FRAGMENT_SHADER, f_src);

	if (rvs.status == ERROR) {
		return Result_Shader_err(
			Result_u32_unwrap_err(rvs)
		);
	}
	if (rfs.status == ERROR) {
		return Result_Shader_err(
			Result_u32_unwrap_err(rfs)
		);
	}

	u32 vs = Result_u32_unwrap(rvs);
	u32 fs = Result_u32_unwrap(rfs);

	// Attaching shader
	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return Result_Shader_ok(program);
}

void shader_delete(Shader id) {
	GLCall(glDeleteProgram(id));
}

Result_u32 shader_compile(Shader_Type type, const char* shader_src) {
	u32 id = glCreateShader(type);
	GLCall(glShaderSource(id, 1, &shader_src, NULL));
	GLCall(glCompileShader(id));

	// Checking error in shader
	i32 result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE) {
		i32 length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));

		// TODO: This memory is not freed.
		char message[length];
		GLCall(glGetShaderInfoLog(id, length, &length, message));

		char* buffer = (char*) calloc(length + 100, sizeof(char));
		sprintf(buffer, "Failed to compile [%s shader]\n%s\n",
			(type == GL_VERTEX_SHADER ? "Vertex" : "Fragment"), message
		);

		return Result_u32_err(buffer);
	}
	return Result_u32_ok(id);
}

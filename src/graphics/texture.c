#include "stb_image.h"
#include "texture.h"
#include "GL/glew.h"
#include "core/log.h"

DEFINE_RESULT(Texture, Texture);

Result_Texture texture_from_file(const char* filepath) {
	stbi_set_flip_vertically_on_load(1);

	i32 w, h, c;
	u8* data = stbi_load(filepath, &w, &h, &c, 0);
	if (!data) {
		return Result_Texture_err("Failed to load texture file");
	}

	// Binding the texture
	u32 id;
	GLCall(glGenTextures(1, &id));
	GLCall(glBindTexture(GL_TEXTURE_2D, id));

	// Setting up some basic modes to display texture
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	// Sending the pixel data to opengl
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	if (data) {
		stbi_image_free(data);
	}

	return Result_Texture_ok((Texture) {
		id, w, h
	});
}

Result_Texture texture_from_data(u32 width, u32 height, u32* data) {
	if (!data) {
		return Result_Texture_err("Cannot create texture from null data");
	}

	u32 id;
	GLCall(glGenTextures(1, &id));
	GLCall(glBindTexture(GL_TEXTURE_2D, id));
	
	// Setting up some basic modes to display texture
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	
	// Sending the pixel data to opengl
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	return Result_Texture_ok((Texture) {
		id, width, height
	});
}

void texture_bind(Texture texture) {
	GLCall(glBindTextureUnit(texture.id, texture.id));
}

void texture_unbind(Texture texture) {
	GLCall(glBindTextureUnit(texture.id, 0));
}

void texture_delete(Texture texture) {
	GLCall(glDeleteTextures(1, &texture.id));
}

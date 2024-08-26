#include "fbo.h"

Result_FBO fbo_new(u32 width, u32 height) {
	u32 id;

	// Generate and bind framebuffer
	GLCall(glGenFramebuffers(1, &id));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, id));

	// Color texture
	Result_Texture r_color_tex = texture_from_data(width, height, NULL);
	if (r_color_tex.status == ERROR) {
		return ERR(FBO, unwrap_err(r_color_tex));
	}

	Texture color_texture = unwrap(r_color_tex);
	GLCall(glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, color_texture.id, 0
	));

	// Setting up draw buffers
	GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };
	GLCall(glDrawBuffers(1, attachments));

	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return ERR(FBO, "Framebuffer is not complete!");
	}

	return OK(FBO, (FBO) {
		.id = id,
		.color_texture = color_texture,
	});
}

void fbo_delete(FBO* fbo) {
	texture_delete(fbo->color_texture);
	GLCall(glDeleteFramebuffers(1, &fbo->id));
}

void fbo_bind(FBO* fbo) {
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, fbo->id));
}

void fbo_unbind() {
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

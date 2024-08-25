#ifndef __FBO_H__
#define __FBO_H__

#include "core/defines.h"
#include "core/result.h"
#include "core/log.h"
#include "texture.h"

typedef struct {
	u32 id;
	Texture color_texture;
} FBO;

RESULT(FBO, FBO);

Result_FBO fbo_new(u32 width, u32 height);
void fbo_delete(FBO* fbo);
void fbo_bind(FBO* fbo);
void fbo_unbind();

#endif // __FBO_H__

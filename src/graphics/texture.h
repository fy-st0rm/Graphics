#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "core/defines.h"
#include "core/result.h"

typedef struct {
	u32 id, width, height;
} Texture;

DECLARE_RESULT(Texture, Texture);

// TODO: Implement control over texture filters
// Filters are hard coded for now
Result_Texture texture_from_file(const char* filepath);
Result_Texture texture_from_data(u32 width, u32 height, u32* data);
void texture_bind(Texture texture);
void texture_unbind(Texture texture);
void texture_delete(Texture texture);

#endif // __TEXTURE_H__

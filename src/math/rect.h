#ifndef __RECT_H__
#define __RECT_H__

#include "vec.h"

typedef struct {
	f32 x, y, w, h;
} Rect;

#define print_rect(r) print_v4(*((v4*)&r))

#endif // __RECT_H__


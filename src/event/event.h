#ifndef __EVENT_H__
#define __EVENT_H__

#include "core/defines.h"
#include "core/dyn_array.h"
#include "window/window.h"
#include "math/vec.h"

typedef enum {
	NONE,

	KEYDOWN,
	KEYUP,

	MOUSE_BUTTON_DOWN,
	MOUSE_BUTTON_UP,
	MOUSE_MOTION,
} Event_Type;

typedef enum {
	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_RIGHT
} Mouse_Button;

typedef struct {
	Event_Type type;

	union {
		u32 key;
		Mouse_Button button;
		v2 mouse_pos;
	} e;
} Event;

i32 event_poll(Window window, Event* event);
v2 event_mouse_pos(Window window);
void event_set_mouse_pos(Window window, v2 pos);

#endif // __EVENT_H__

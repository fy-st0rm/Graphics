#ifndef __EVENT_H__
#define __EVENT_H__

#include "core/defines.h"
#include "core/dyn_array.h"
#include "window/window.h"

typedef enum {
	NONE,

	KEYDOWN,
	KEYUP,

	MOUSE_BUTTON_DOWN,
	MOUSE_BUTTON_UP,
} Event_Type;

typedef enum {
	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_RIGHT
} Mouse_Button;

typedef struct {
	Event_Type type;
	u32 key;
	Mouse_Button button;
} Event;

i32 event_poll(Window window, Event* event);

#endif // __EVENT_H__

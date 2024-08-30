#ifndef __CTX_H__
#define __CTX_H__

#include "trace_allocator.h"
#include "dyn_array.h"
#include "event/event.h"

typedef struct {
	Dyn_Array(Event) events;
	Trace_Allocator* trace_allocator;
} Context;

Context* ctx_new();
void ctx_delete(Context* ctx);

#endif // __CTX_H__

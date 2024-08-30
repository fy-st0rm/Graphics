#include "alloc.h"
#include "ctx.h"

extern Context* ctx;

void* __alloc(i64 size, const char* file, i32 line) {
	return __trace_allocator_alloc(ctx->trace_allocator, size, file, line);
}

void clean(void* ptr) {
	trace_allocator_free(ctx->trace_allocator, ptr);
}

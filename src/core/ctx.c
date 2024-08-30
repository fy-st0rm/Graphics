#include "ctx.h"

Context* ctx_new() {
	Context* ctx = (Context*) malloc(sizeof(Context));
	ctx->events = NULL;
	ctx->trace_allocator = trace_allocator_new();
	return ctx;
}

void ctx_delete(Context* ctx) {
	dyn_array_delete(ctx->events);
	trace_allocator_delete(ctx->trace_allocator);
	free(ctx);
}

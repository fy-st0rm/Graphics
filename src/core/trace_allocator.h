#ifndef __TRACE_ALLOCATOR_H__
#define __TRACE_ALLOCATOR_H__

#include "core/defines.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
	void* ptr;
	size_t size;
	const char* file;
	i32 line;
} Traceable_Memory_Block;

void print_traceable_memory_block(Traceable_Memory_Block* block);

#define TRACE_ALLOCATOR_MEMORY_CAP 1024
#define TRACE_ALLOCATOR_GROW_RATE 0.5
typedef struct {
	Traceable_Memory_Block* blocks;
	u32 blocks_cnt;
	u32 blocks_cap;
} Trace_Allocator;

Trace_Allocator* trace_allocator_new();
void trace_allocator_delete(Trace_Allocator* allocator);

#define trace_allocator_alloc(allocator, size) __trace_allocator_alloc(allocator, size, __FILE__, __LINE__)
void* __trace_allocator_alloc(Trace_Allocator* allocator, size_t size, const char* file, i32 line);
void trace_allocator_free(Trace_Allocator* allocator, void* ptr);
void trace_allocator_alert(Trace_Allocator* allocator);


#endif // __TRACE_ALLOCATOR_H__

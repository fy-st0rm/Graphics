#include "trace_allocator.h"

void print_traceable_memory_block(Traceable_Memory_Block* block) {
	printf("%p at %s:%d of %zu bytes\n", block->ptr, block->file, block->line, block->size);
}

Trace_Allocator* trace_allocator_new() {
	Trace_Allocator* allocator = (Trace_Allocator*) calloc(1, sizeof(Trace_Allocator));

	allocator->blocks = (Traceable_Memory_Block*) calloc(
		TRACE_ALLOCATOR_MEMORY_CAP,
		sizeof(Traceable_Memory_Block)
	);
	allocator->blocks_cnt = 0;
	allocator->blocks_cap = TRACE_ALLOCATOR_MEMORY_CAP;

	return allocator;
}

void trace_allocator_delete(Trace_Allocator* allocator) {
	trace_allocator_alert(allocator);
	free(allocator->blocks);
	free(allocator);
}

void handle_blocks_overflow(Trace_Allocator* allocator) {
	if (allocator->blocks_cnt < allocator->blocks_cap) return;

	Traceable_Memory_Block* tmp_blocks = (Traceable_Memory_Block*) calloc(
		TRACE_ALLOCATOR_MEMORY_CAP,
		sizeof(Traceable_Memory_Block)
	);
	memcpy(
		tmp_blocks, allocator->blocks,
		sizeof(Traceable_Memory_Block) * allocator->blocks_cnt
	);

	free(allocator->blocks);

	allocator->blocks_cap += TRACE_ALLOCATOR_GROW_RATE * TRACE_ALLOCATOR_MEMORY_CAP;
	allocator->blocks = (Traceable_Memory_Block*) calloc(
		allocator->blocks_cap,
		sizeof(Traceable_Memory_Block)
	);
	memcpy(
		allocator->blocks, tmp_blocks,
		sizeof(Traceable_Memory_Block) * allocator->blocks_cnt
	);

	free(tmp_blocks);
}

void* __trace_allocator_alloc(Trace_Allocator* allocator, size_t size, const char* file, i32 line) {
	handle_blocks_overflow(allocator);

	void* ptr = (void*) malloc(size);
	Traceable_Memory_Block mem = {
		ptr,
		size,
		file,
		line
	};
	memset(ptr, 0, size);

	allocator->blocks[allocator->blocks_cnt++] = mem;
	return ptr;
}

void trace_allocator_free(Trace_Allocator* allocator, void* ptr) {
	for (u32 i = 0; allocator->blocks_cnt; i++) {
		if (allocator->blocks[i].ptr == ptr) {
			memmove(
				&allocator->blocks[i], &allocator->blocks[i+1],
				(allocator->blocks_cnt - i - 1) * sizeof(Traceable_Memory_Block)
			);
			allocator->blocks_cnt--;

			free(ptr);
			break;
		}
	}
}

void trace_allocator_alert(Trace_Allocator* allocator) {
	if (!allocator->blocks_cnt) return;

	printf("\n---------Unfreed memories---------\n");
	size_t total = 0;
	for (u32 i = 0; i < allocator->blocks_cnt; i++) {
		print_traceable_memory_block(&allocator->blocks[i]);
		total += allocator->blocks[i].size;
	}
	printf("\nTotal unfreed memories count = %d\n", allocator->blocks_cnt);
	printf("Total unfreed memories = %d bytes\n", total);
	printf("---------Unfreed memories---------\n\n");
}

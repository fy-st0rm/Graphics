#ifndef __ALLOC_H__
#define __ALLOC_H__

#include "defines.h"

#define alloc(size) __alloc(size, __FILE__, __LINE__)
void* __alloc(i64 size, const char* file, i32 line);
void clean(void* ptr);

#endif // __ALLOC_H__

#ifndef __DYN_ARRAY_H__
#define __DYN_ARRAY_H__

#include "defines.h"
#include "log.h"
#include "alloc.h"

#include <stdlib.h>
#include <string.h>

#define DYN_ARR_MEMORY_CAP 100
#define DYN_ARR_GROW_RATE 0.5

#define Dyn_Array(T) \
	struct {           \
		T* data;         \
		T dummy;         \
		int cap;         \
		int len;         \
	}*                 \

#define dyn_array_delete(arr) \
	do {                        \
		if ((arr)) {              \
			clean((arr)->data);   \
			clean((arr));         \
		}                         \
	} while (0)                 \

#define dyn_array_check_cap(arr)                                     \
	do {                                                               \
		if ((arr)->len >= (arr)->cap) {                                  \
			void* tmp = alloc(sizeof((arr)->dummy) * (arr)->len);       \
			memcpy(tmp, (arr)->data, sizeof((arr)->dummy) * (arr)->len);   \
			clean((arr)->data);                                             \
                                                                     \
			(arr)->cap += DYN_ARR_GROW_RATE * DYN_ARR_MEMORY_CAP;          \
			(arr)->data = alloc(sizeof((arr)->dummy) * (arr)->cap);     \
			memcpy((arr)->data, tmp, sizeof((arr)->dummy) * (arr)->len);   \
			clean(tmp);                                                  \
		}                                                                \
	} while (0)                                                        \

#define dyn_array_len(arr)      \
	({                            \
		i32 len = 0;                \
		if ((arr)) len = arr->len;  \
		len;                        \
	})                            \

#define dyn_array_cap(arr)     \
	({                           \
		i32 cap = 0;               \
		if ((arr)) cap = arr->cap; \
		cap;                       \
	})                           \

#define dyn_array_append(arr, ...)                                   \
	do {                                                               \
		if ((arr) == NULL) {                                             \
			(arr) = alloc(sizeof(*(arr)));                              \
			(arr)->len = 0;                                                \
			(arr)->cap = DYN_ARR_MEMORY_CAP;                               \
			(arr)->data = alloc(sizeof((arr)->dummy) * (arr)->cap);     \
		}                                                                \
		dyn_array_check_cap(arr);                                        \
		(arr)->data[(arr)->len++] = (__VA_ARGS__);                                 \
	} while (0)                                                        \

#define dyn_array_get(arr, idx)                                                                            \
	({                                                                                                       \
		assert(idx < (arr)->len, "Tried accessing index: %d to an array of length: %d\n", idx, (arr)->len); \
		(arr)->data[idx];                                                                                      \
	})                                                                                                       \

#define dyn_array_get_ref(arr, idx)                                                                            \
	({                                                                                                       \
		assert(idx < (arr)->len, "Tried accessing index: %d to an array of length: %d\n", idx, (arr)->len); \
		&(arr)->data[idx];                                                                                      \
	})                                                                                                       \

#define dyn_array_pop(arr, idx)                                                                            \
	({                                                                                                       \
		assert(idx < (arr)->len, "Tried poping index: %d to an array of length: %d\n", idx, (arr)->len);    \
		(arr)->dummy = dyn_array_get(arr, idx);                                                                \
		memmove((arr)->data + idx, (arr)->data + idx + 1, sizeof((arr)->dummy) * ((arr)->len - idx));          \
		(arr)->len--;                                                                                          \
		(arr)->dummy;                                                                                          \
	})                                                                                                       \

#define dyn_array_exists(arr, e)           \
	({                                       \
		bool res = false;                      \
		for (int i = 0; i < (arr)->len; i++) { \
			if ((arr)->data[i] == (e)) {         \
				res = true;                        \
				break;                             \
			}                                    \
		}                                      \
		res;                                   \
	})                                       \

#define dyn_array_index_of(arr, e)                                                                        \
	({                                                                                                      \
		assert(dyn_array_exists(arr, e), "Tried getting the index of an element that doesn't exists.\n");  \
		int res = -1;                                                                                         \
		for (int i = 0; i < (arr)->len; i++) {                                                                \
			if ((arr)->data[i] == (e)) {                                                                        \
				res = i;                                                                                          \
				break;                                                                                            \
			}                                                                                                   \
		}                                                                                                     \
		res;                                                                                                  \
	})                                                                                                      \

#define dyn_array_for_each(arr, x, block)  \
	do {                                     \
		for (int i = 0; i < (arr)->len; i++) { \
			x = dyn_array_get(arr, i);           \
			block                                \
		}                                      \
	} while (0)                              \

#define dyn_array_for_each_ref(arr, x, block)  \
	do {                                         \
		for (int i = 0; i < (arr)->len; i++) {     \
			x = dyn_array_get_ref(arr, i);           \
			block                                    \
		}                                          \
	} while (0)                                  \


#endif // __DYN_ARRAY_H__

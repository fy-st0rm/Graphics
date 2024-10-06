#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#include "defines.h"
#include "log.h"
#include "alloc.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// Ignoring the incoatible-pointer-types error
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"

#define MAX_HMAP_SIZE 1024
#define COMPARE(x, y, size) ({\
	int r = 0;\
	if (IS_POINTER(x)) {\
		r = (memcmp((void*)x, (void*) y, size) == 0);\
	} else {\
		r = (x == y);\
	}\
	r;\
})\


int hash_data(char* data) {
	int hash = 5381;
	int c;
	while ((c = *data++)) {
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}

#define __hashmap_entry(K, V)\
	struct {\
		K key;\
		V value;\
		void* parent;\
		void* child;\
	}\

#define Hashmap(K, V) \
	struct {\
		__hashmap_entry(K, V)* data[MAX_HMAP_SIZE];\
	}*\

#define hashmap_insert(map, k, v) ({\
/* Initialization of map */\
	if (map == NULL) {\
		map = alloc(sizeof(*map));\
		memset(map, 0, sizeof(*map));\
	}\
/* Calculating hash */\
	__typeof__(k) tmp_k = k;\
	int hash;\
	if (IS_POINTER(tmp_k)) {\
		hash = hash_data((char*) tmp_k);\
	} else {\
		hash = hash_data((char*) &tmp_k);\
	}\
	int idx = hash % MAX_HMAP_SIZE;\
/* Starting child and parent */\
	__typeof__(map->data[idx])* child = &map->data[idx];\
	__typeof__(map->data[idx]) parent = NULL;\
/* Looping over the childs until the child is NULL or the key matched */\
	while (*child && !COMPARE((*child)->key, tmp_k, sizeof(tmp_k))) {\
		parent = *child;\
		child = (__typeof__(map->data[idx])*) &(*child)->child;\
	}\
/* If the child already exists just replace the value */\
	if (*child != NULL) {\
		(*child)->value = v;\
	} else {\
/* Creating new child */\
		*child = alloc(sizeof(__typeof__(*map->data[idx])));\
		(*child)->key = k;\
		(*child)->value = v;\
		(*child)->parent = parent;\
		(*child)->child = NULL;\
	}\
})\

#define hashmap_get_entry(map, k, res) ({\
	res = true;\
/* Calculating hash */\
	__typeof__(k) tmp_k = k;\
	int hash;\
	if (IS_POINTER(tmp_k)) {\
		hash = hash_data((char*) tmp_k);\
	} else {\
		hash = hash_data((char*) &tmp_k);\
	}\
	int idx = hash % MAX_HMAP_SIZE;\
	__typeof__(map->data[idx])* child = &map->data[idx];\
/* Looping over the childs until the child is NULL or the key matched */\
	while (*child && !COMPARE((*child)->key, tmp_k, sizeof(tmp_k))) {\
		child = (__typeof__(map->data[idx])*) &(*child)->child;\
	}\
	if (*child == NULL) {\
		res = false;\
	}\
	(*child);\
})

#define hashmap_get(map, k) ({\
	bool res = true;\
	__typeof__(map->data[0]) entry = hashmap_get_entry(map, k, res);\
	if (res == false) {\
		printf("Cannot find\n");\
		exit(1);\
	}\
	entry->value;\
})

#define hashmap_exists(map, k) ({\
	bool exists = true;\
	hashmap_get_entry(map, k, exists);\
	exists;\
})

#define hashmap_pop(map, k) ({\
	bool res = true;\
	__typeof__(map->data[0]) entry = hashmap_get_entry(map, k, res);\
	/* Checking if entry exists or not */\
	if (res == false) {\
		printf("Cannot find\n");\
		exit(1);\
	}\
	__typeof__(map->data[0]) parent = entry->parent;\
	__typeof__(map->data[0]) child = entry->child;\
	if (parent != NULL) {\
		parent->child = child;\
	}\
	else if (parent == NULL) {\
		/* Calculating hash */\
		__typeof__(k) tmp_k = k;\
		int hash;\
		if (IS_POINTER(tmp_k)) {\
			hash = hash_data((char*) tmp_k);\
		} else {\
			hash = hash_data((char*) &tmp_k);\
		}\
		int idx = hash % MAX_HMAP_SIZE;\
		map->data[idx] = child;\
	}\
	__typeof__(entry->value) value = entry->value;\
	clean(entry);\
	value;\
})

#define hashmap_print(map, k_fmt, v_fmt)                                                        \
	do {                                                                                           \
		printf("\n---HASHMAP START---\n");\
		for (int i = 0; i < MAX_HMAP_SIZE; i++) {                                              \
			if (map->data[i]) {                                                               \
				printf("%d: (" k_fmt ", " v_fmt ") ", i, map->data[i]->key, map->data[i]->value);        \
				__typeof__(map->data[i])* child = (__typeof__(map->data[i])*) &map->data[i]->child;\
				while (*child != NULL) {\
					printf("| (" k_fmt ", " v_fmt ") ", (*child)->key, (*child)->value);        \
					child = (__typeof__(map->data[i])*) &(*child)->child;\
				}\
				printf("\n");                                                                            \
			}                                                                                          \
		}                                                                                            \
		printf("---HASHMAP END---\n\n");                                                                                \
	} while (0)


#define hashmap_delete(map) ({\
	for (int i = 0; i < MAX_HMAP_SIZE; i++) {\
		if (map->data[i]) {\
			__typeof__(map->data[i])* child = (__typeof__(map->data[i])*) &map->data[i]->child;\
			while (*child != NULL) {\
				__typeof__(map->data[i]) t = *child;\
				child = (__typeof__(map->data[i])*) &(*child)->child;\
				clean(t);\
				t = NULL;\
			}\
			clean(map->data[i]);\
			map->data[i] = NULL;\
		}\
	}\
})

#endif //  __HASHMAP_H__

#ifndef __RESULT_H__
#define __RESULT_H__

#include "log.h"

typedef enum {
	OK, ERROR
} ResultStatus;

#define DECLARE_RESULT(name, V)                               \
	typedef struct {                                            \
		ResultStatus status;                                      \
		union {                                                   \
			V result;                                               \
			const char* error;                                      \
		} __value;                                                \
	} Result_##name;                                            \
	Result_##name Result_##name##_ok(V v);                      \
	Result_##name Result_##name##_err(const char* e);           \
	V Result_##name##_unwrap(Result_##name res);                \
	V Result_##name##_unwrap_or(Result_##name res, V x);        \
	const char* Result_##name##_unwrap_err(Result_##name res);  \

#define DEFINE_RESULT(name, V)                                \
	Result_##name Result_##name##_ok(V v) {                     \
		return (Result_##name) {                                  \
			.status = OK,                                           \
			.__value.result = v                                     \
		};                                                        \
	}                                                           \
                                                              \
	Result_##name Result_##name##_err(const char* e) {          \
		return (Result_##name) {                                  \
			.status = ERROR,                                        \
			.__value.error = e                                      \
		};                                                        \
	}                                                           \
                                                              \
	V Result_##name##_unwrap(Result_##name res) {               \
		if (res.status == OK) return res.__value.result;          \
		assert(0, "%s\n", res.__value.error);                     \
	}                                                           \
                                                              \
	V Result_##name##_unwrap_or(Result_##name res, V x) {       \
		if (res.status == OK) return res.__value.result;          \
		return x;                                                 \
	}                                                           \
	                                                            \
	const char* Result_##name##_unwrap_err(Result_##name res) { \
		if (res.status == ERROR) return res.__value.error;        \
		assert(0, "Result was not an error\n");                   \
	}                                                           \


#endif // __RESULT_H__

#ifndef __RESULT_H__
#define __RESULT_H__

#include "log.h"

typedef enum {
	OK, ERROR
} ResultStatus;

#define RESULT(name, V)                                       \
	typedef struct {                                            \
		ResultStatus status;                                      \
		union {                                                   \
			V result;                                               \
			const char* error;                                      \
		} __value;                                                \
	} Result_##name;                                            \

#define OK(res_name, ...) ({                                  \
	(Result_##res_name) {                                       \
		.status = OK,                                             \
		.__value.result = (__VA_ARGS__)                           \
	};                                                          \
})                                                            \

#define ERR(res_name, e) ({                                   \
	(Result_##res_name) {                                       \
		.status = ERROR,                                          \
		.__value.error = e                                        \
	};                                                          \
})                                                            \

#define unwrap(res) ({                                        \
	__typeof__(res) r = res;                                    \
	assert(r.status == OK, "%s\n", r.__value.error);            \
	r.__value.result;                                           \
})                                                            \

#define unwrap_or(res, x) ({                                  \
	__typeof__(res) r = res;                                    \
	__typeof__(r.__value.result) v = x;                         \
	if (r.status == OK) v = res.__value.result;                 \
	v;                                                          \
})                                                            \

#define unwrap_err(res) ({                                    \
	__typeof__(res) r = res;                                    \
	assert(r.status == ERROR, "Result was not an error\n");     \
	r.__value.error;                                            \
})                                                            \


#endif // __RESULT_H__

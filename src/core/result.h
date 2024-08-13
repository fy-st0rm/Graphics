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
	assert(res.status == OK, "%s\n", res.__value.error);        \
	res.__value.result;                                         \
})                                                            \

#define unwrap_or(res, x) ({                                  \
	r = x;                                                      \
	if (res.status == OK) r = res.__value.result;               \
	r;                                                          \
})                                                            \

#define unwrap_err(res) ({                                    \
	assert(res.status == ERROR, "Result was not an error\n");   \
	res.__value.error;                                          \
})                                                            \


#endif // __RESULT_H__

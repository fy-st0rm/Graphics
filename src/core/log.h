#ifndef __log_H__
#define __log_H__

#include "GL/glew.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * @brief: Assertion macro
 */

// Properly define static assertions.
#if defined(__clang__) || defined(__gcc__)
/** @brief Static assertion */
#define STATIC_ASSERT _Static_assert
#else

/** @brief Static assertion */
#define STATIC_ASSERT static_assert
#endif


#define assert(x, ...) \
	do {\
		if (!(x)) {\
			fprintf(stderr, "\033[31m[ASSERTION]: %s:%d:\033[0m ", __FILE__, __LINE__);\
			fprintf(stderr, __VA_ARGS__);\
			exit(1);\
		}\
	} while(0);\

/*
 * @brief: loging color codes
 */

#define LOG_NORMAL 0
#define LOG_ERROR  91
#define LOG_SUCESS 92
#define LOG_WARN   93
#define LOG_INFO   95

/*
 * @brief: loging macros
 */

#define log_typed(type, ...)         \
	({                                     \
		printf("\033[%dm", type);            \
		printf(__VA_ARGS__);                 \
		printf("\033[%dm", LOG_NORMAL);   \
	})

//#define log(...)                             \
//	({                                            \
//		log_typed(LOG_NORMAL, "[log]:    ");  \
//		log_typed(LOG_NORMAL, __VA_ARGS__);   \
//	})

#define log_sucess(...)                      \
	({                                            \
		log_typed(LOG_SUCESS, "[SUCESS]: ");  \
		log_typed(LOG_SUCESS, __VA_ARGS__);   \
	})

#define log_warn(...)                        \
	({                                            \
		log_typed(LOG_WARN,   "[WARN]:   ");  \
		log_typed(LOG_WARN, __VA_ARGS__);     \
	})

#define log_error(...)                       \
	({                                            \
		log_typed(LOG_ERROR,  "[ERROR]:  ");  \
		log_typed(LOG_ERROR, __VA_ARGS__);    \
	})

#define log_info(...)                        \
	({                                            \
		log_typed(LOG_INFO,   "[INFO]:   ");  \
		log_typed(LOG_INFO, __VA_ARGS__);     \
	})


/*
 * @brief Macros and functions for opengl error handling
 */

#define GLCall(x)\
	(\
		clear_gl_error(), \
		x\
	);\
	assert(gl_error_log(#x, __FILE__, __LINE__), "Opengl failed.\n");\

static void clear_gl_error() {
	while(glGetError());
}

static int gl_error_log(const char* function, const char* file, int line) {
	GLenum error;
	while ((error = glGetError())) {
		log_error("[Error code]: %d\n", error);
		log_error("[Error message]: %s\n", gluErrorString(error));
		log_error("[Opengl error]: %s %s: %d\n", function ,file, line);
		return 0;
	}
	return 1;
}

#endif // __LOG_H__

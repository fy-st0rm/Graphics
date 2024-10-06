#ifndef __DEFINES_H__
#define __DEFINES_H__

#include "result.h"

/** @brief Unsigned 8-bit integer */
typedef unsigned char u8;

/** @brief Unsigned 16-bit integer */
typedef unsigned short u16;

/** @brief Unsigned 32-bit integer */
typedef unsigned int u32;

/** @brief Unsigned 64-bit integer */
typedef unsigned long long u64;

// Signed int types.

/** @brief Signed 8-bit integer */
typedef signed char i8;

/** @brief Signed 16-bit integer */
typedef signed short i16;

/** @brief Signed 32-bit integer */
typedef signed int i32;

/** @brief Signed 64-bit integer */
typedef signed long long i64;

// Floating point types

/** @brief 32-bit floating point number */
typedef float f32;

/** @brief 64-bit floating point number */
typedef double f64;

// Boolean types

/** @brief 32-bit boolean type, used for APIs which require it */
typedef enum {
	false, true
} b32;

/* @brief Macro to check if the variable is pointer or not. */
#define IS_POINTER(x) _Generic((x),\
          int: 0,       unsigned int: 0,\
         long: 0,      unsigned long: 0,\
    long long: 0, unsigned long long: 0,\
        float: 0,             double: 0,\
  long double: 0,                       \
      default: 1)


// Declaring all the results for basic types
RESULT(u8, u8);
RESULT(u16, u16);
RESULT(u32, u32);
RESULT(u64, u64);

RESULT(i8, i8);
RESULT(i16, i16);
RESULT(i32, i32);
RESULT(i64, i64);

RESULT(f32, f32);
RESULT(f64, f64);

RESULT(b32, b32);

#endif // __DEFINES_H__

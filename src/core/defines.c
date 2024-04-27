#include "defines.h"

// Ensure all types are of the correct size.

/** @brief Assert u8 to be 1 byte.*/
STATIC_ASSERT(sizeof(u8) == 1, "Expected u8 to be 1 byte.");

/** @brief Assert u16 to be 2 bytes.*/
STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 to be 2 bytes.");

/** @brief Assert u32 to be 4 bytes.*/
STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 to be 4 bytes.");

/** @brief Assert u64 to be 8 bytes.*/
STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 to be 8 bytes.");

/** @brief Assert i8 to be 1 byte.*/
STATIC_ASSERT(sizeof(i8) == 1, "Expected i8 to be 1 byte.");

/** @brief Assert i16 to be 2 bytes.*/
STATIC_ASSERT(sizeof(i16) == 2, "Expected i16 to be 2 bytes.");

/** @brief Assert i32 to be 4 bytes.*/
STATIC_ASSERT(sizeof(i32) == 4, "Expected i32 to be 4 bytes.");

/** @brief Assert i64 to be 8 bytes.*/
STATIC_ASSERT(sizeof(i64) == 8, "Expected i64 to be 8 bytes.");

/** @brief Assert f32 to be 4 bytes.*/
STATIC_ASSERT(sizeof(f32) == 4, "Expected f32 to be 4 bytes.");

/** @brief Assert f64 to be 8 bytes.*/
STATIC_ASSERT(sizeof(f64) == 8, "Expected f64 to be 8 bytes.");

/** @brief Assert b32 to be 4 bytes.*/
STATIC_ASSERT(sizeof(b32) == 4, "Expected b32 to be 4 bytes.");

/** @brief Assert b8 to be 1 bytes.*/
STATIC_ASSERT(sizeof(b8) == 1, "Expected b8 to be 1 bytes.");


// Defining the results for basic types
DEFINE_RESULT(u8, u8);
DEFINE_RESULT(u16, u16);
DEFINE_RESULT(u32, u32);
DEFINE_RESULT(u64, u64);

DEFINE_RESULT(i8, i8);
DEFINE_RESULT(i16, i16);
DEFINE_RESULT(i32, i32);
DEFINE_RESULT(i64, i64);

DEFINE_RESULT(f32, f32);
DEFINE_RESULT(f64, f64);

DEFINE_RESULT(b8, b8);
DEFINE_RESULT(b32, b32);

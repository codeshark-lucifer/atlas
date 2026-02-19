#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef const char* str;

// ============================
// File read/write utilities
// ============================

// Reads entire file into a null-terminated string.
// Returns malloc'ed char* on success, nullptr on failure.
// The caller must free() the result.
char* read_file(str path);

// Writes buffer to file. Returns 1 on success, 0 on failure.
int write_file(str path, const char* buffer, u64 size);

// Simple printf wrapper
void print(str fmt, ...);

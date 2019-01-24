#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef NULL
#define NULL ((void*) 0)
#endif

// Explicitly-sized versions of integer types
typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef unsigned long   u_long;
typedef unsigned int    u_int;
typedef unsigned short  u_short;
typedef unsigned char   u_char;
typedef unsigned long long 		u_ll;

// Pointers and addresses are 32 bits long.
// We use pointer types to represent virtual addresses,
// uintptr_t to represent the numerical values of virtual addresses,
// and physaddr_t to represent physical addresses.
typedef int32_t intptr_t;
typedef uint32_t uintptr_t;
typedef uint32_t physaddr_t;

// Page numbers are 32 bits long.
typedef uint32_t ppn_t;

// size_t is used for memory object sizes.
typedef uint32_t size_t;

#define MAX_ORDER 1024

// Efficient min and max operations
#define MIN(_a, _b)						    \
({								            \
	typeof(_a) __a = (_a);					\
	typeof(_b) __b = (_b);					\
	__a <= __b ? __a : __b;					\
})
#define MAX(_a, _b)						    \
({								            \
	typeof(_a) __a = (_a);					\
	typeof(_b) __b = (_b);					\
	__a >= __b ? __a : __b;					\
})

// Rounding operations (efficient when n is a power of 2)
#define ROUND(a, n)     (((((u_long)(a)) + (n) - 1)) & ~((n) - 1))
// Round down to the nearest multiple of n
#define ROUNDDOWN(a, n)						\
({								            \
	uint32_t __a = (uint32_t) (a);			\
	(typeof(a)) (__a - __a % (n));			\
})
// Round up to the nearest multiple of n
#define ROUNDUP(a, n)						                \
({								                            \
	uint32_t __n = (uint32_t) (n);				            \
	(typeof(a)) (ROUNDDOWN((uint32_t) (a) + __n - 1, __n));	\
})

#define ARRAY_SIZE(a)	(sizeof(a) / sizeof(a[0]))

// Return the offset of 'member' relative to the beginning of a struct type
#ifndef offsetof
#define offsetof(type, member)  ((size_t) (&((type*)0)->member))
#endif

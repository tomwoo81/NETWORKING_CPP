#ifndef _COMMON_TYPE_H_
#define _COMMON_TYPE_H_

#include <stdbool.h>
#include <inttypes.h>

typedef int8_t S8;
typedef uint8_t U8;

typedef int16_t S16;
typedef uint16_t U16;

typedef int32_t S32;
typedef uint32_t U32;

typedef int64_t S64;
typedef uint64_t U64;

#define STATUS_OK    0
#define STATUS_ERR (-1)
typedef S32 STATUS;

#endif

/* End of File */

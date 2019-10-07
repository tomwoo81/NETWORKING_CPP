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

//The following rules are defined in [RFC2616] (¡°Hypertext Transfer Protocol -- HTTP/1.1¡±).
#define OCTET_STRING_MAX_LEN 128 /* 128 Bytes */
typedef struct
{
	U8 value[OCTET_STRING_MAX_LEN];
	U32 length;
} OctetString;
//OCTET = <any 8-bit sequence of data>

#define TEXT_STRING_MAX_LEN 128 /* 128 Bytes */
typedef struct
{
	char value[TEXT_STRING_MAX_LEN + 1];
} TextString;
//TEXT = <any OCTET except CTLs, but including LWS>
//CTL = <any US-ASCII control character (octets 0 - 31) and DEL (127)>
//LWS = [CRLF] 1*( SP | HT )
//CRLF = CR LF
//CR = <US-ASCII CR, carriage return (13)>
//LF = <US-ASCII LF, linefeed (10)>
//SP = <US-ASCII SP, space (32)>
//HT = <US-ASCII HT, horizontal-tab (9)>

#endif

/* End of File */

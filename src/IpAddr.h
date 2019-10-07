/******************************************************************************
  FileName:      ipAddr.h
  Author:        Tom Woo (wuzhiming)
  Version :      4.2
  Date:          2015.12.05
  Description:   Definition of IP Address.
  History:
      <author>             <date>      <version>            <desc>
  Tom Woo (wuzhiming)    2015.11.02       4.0                New.
******************************************************************************/

#ifndef _IP_ADDR_H_
#define _IP_ADDR_H_

#include "CommonTypes.h"

#define IP_ADDR_VER_4  4 /* IPv4 (4-octet address) */
#define IP_ADDR_VER_6 16 /* IPv6 (16-octet address) */

typedef struct IpAddr
{
    U32 ver;      /* Version */
    U8  addr[16]; /* Address (IPv4: 32 bits, initial 4 octets used; IPv6: 128 bits, all 16 octets used) */
} S_IpAddr;

#endif

/* end of file */

/******************************************************************************
  FileName:      socketAdapter.h
  Author:        Tom Woo (wuzhiming)
  Version:       4.10
  Date:          2016.09.27
  Description:   .
  History:
      <author>             <date>      <version>            <desc>
  Tom Woo (wuzhiming)    2015.04.24       4.0                New.
******************************************************************************/

#ifndef _SOCKET_ADAPTER_H_
#define _SOCKET_ADAPTER_H_

#include "CommonTypes.h"
#include "IpAddr.h"

#ifdef __cplusplus
extern "C" {
#endif

S32 OpenAndConfigSocket(const U32 ipVer, const S32 socketType, const U32 flag, const U32 rcvBufLen,
		const U32 sndBufLen, const char* const pIpAddr, const U16 portNumber);

STATUS CloseSocket(const S32 socketId);

STATUS ShutdownSocket(const S32 socketId, const S32 how);

STATUS SetFlagToSocket(const S32 socketId, const U32 flag);

STATUS ConfigSocketWithAddrAndPort(const S32 socketId, const U32 ipVer, const char* const pIpAddr, const U16 portNumber);

STATUS ConfigSocketWithTos(const S32 socketId, const U8 tos);

STATUS ListenToTcpClients(const S32 socketId, const S32 maxNumConns);

STATUS ConnectTcpServer(const S32 socketId, const U32 ipVer, const char* const pIpAddr, const U16 portNumber);

S32 AcceptTcpClient(const S32 listenSockId, S_IpAddr* const pIpAddr, U16* const pPortNumber);

#if 0
STATUS IsTcpSocketConnected(const S32 socketId, bool* const pConnected);
#endif

S32 RecvFromTcpSocket(const S32 socketId, void* const pBuffer, const U32 bufLen, const S32 flags);

S32 RecvFromUdpSocket(const S32 socketId, void* const pBuffer, const U32 bufLen,
		const S32 flags, S_IpAddr* const pIpAddr, U16* const pPortNumber);

S32 SendToTcpSocket(const S32 socketId, const void* const pData, const U32 dataLen, const S32 flags);

S32 SendToUdpSocket(const S32 socketId, const void* const pData, const U32 dataLen,
		const S32 flags, const S_IpAddr* const pIpAddr, const U16 portNumber);

#ifdef __cplusplus
}
#endif

#endif

/* End of File */

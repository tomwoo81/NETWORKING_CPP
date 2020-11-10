/******************************************************************************
  FileName:      socketAdapter.c
  Author:        Tom Woo (wuzhiming)
  Version:       4.12
  Date:          2016.09.30
  Description:   .
  Function List:
                 1. .
  History:
      <author>             <date>      <version>           <desc>
  Tom Woo (wuzhiming)    2015.04.24       4.0               New.
******************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include "Logger.h"
#include "SocketAdapter.h"

/******************************************************************************
  Function:     OpenAndConfigSocket()
  Description:  .
  Input:        .
  Output:       .
  Return:       Socket ID.
  Others:       None.
******************************************************************************/
S32 OpenAndConfigSocket(const U32 ipVer, const S32 socketType, const U32 flag,
		const U32 rcvBufLen, const U32 sndBufLen,
		const char* const pIpAddr, const U16 portNumber)
{
	S32 family, socketId;

	if (NULL == pIpAddr)
	{
		LOG_ERROR(MODULE_ID_SOCKET, "Pointer to ipAddr is NULL!\n");
		return STATUS_ERR;
	}

	if (IP_ADDR_VER_4 == ipVer)
	{
		family = AF_INET;
	}
	else if (IP_ADDR_VER_6 == ipVer)
	{
		family = AF_INET6;
	}
	else
	{
		LOG_ERROR(MODULE_ID_SOCKET, "IP Version (%u) unidentified!\n", ipVer);
		return STATUS_ERR;
	}

	/* Open the socket. */
	socketId = socket(family, socketType, 0);
	if (STATUS_ERR == socketId)
	{
		LOG_ERROR(MODULE_ID_SOCKET, "Fail to open the socket! (errNo = [-%d][%s])\n", errno, strerror(errno));
		return STATUS_ERR;
	}

	/* Set flag to the socket. */
	if (STATUS_OK != SetFlagToSocket(socketId, flag))
	{
		LOG_ERROR(MODULE_ID_SOCKET, "Fail to set flag to the socket!\n");
		return STATUS_ERR;
	}

	/* Set the size of the socket-level receive and send buffers to be allocated. */
	if (STATUS_OK != ConfigSocketWithBufLen(socketId, rcvBufLen, sndBufLen))
	{
		LOG_ERROR(MODULE_ID_SOCKET, "Fail to set the size of the socket-level receive and send buffers to be allocated!\n");
		return STATUS_ERR;
	}

	/* Make address reusable. */
	if (STATUS_OK != ConfigSocketAddrReusable(socketId))
	{
		LOG_ERROR(MODULE_ID_SOCKET, "Fail to make address reusable!\n");
		return STATUS_ERR;
	}

	/* Populate local IP address and local port number. */
	if (strlen(pIpAddr) > 0)
	{
		if (STATUS_OK != ConfigSocketWithAddrAndPort(socketId, ipVer, pIpAddr, portNumber))
		{
			LOG_ERROR(MODULE_ID_SOCKET, "Fail to config the socket with local IP address and local port number!\n");
			return STATUS_ERR;
		}
	}

	LOG_INFO(MODULE_ID_SOCKET, "Open and configure a socket (socket ID: %d) successfully.\n", socketId);

	return socketId;
}

/******************************************************************************
  Function:     CloseSocket()
  Description:  Delete the file descriptor of the socket from memory.
  Input:        .
  Output:       .
  Return:       Status.
  Others:       None.
******************************************************************************/
STATUS CloseSocket(const S32 socketId)
{
	if (STATUS_OK != close(socketId))
	{
		LOG_ERROR(MODULE_ID_SOCKET, "Fail to close the socket (socket ID: %d)! (errNo = [-%d][%s])\n", socketId, errno, strerror(errno));
		return STATUS_ERR;
	}

	LOG_INFO(MODULE_ID_SOCKET, "Close the socket (socket ID: %d) successfully.\n", socketId);

	return STATUS_OK;
}

/******************************************************************************
  Function:     ShutdownSocket()
  Description:  Disconnect the I/O stream of the socket.
  Input:        .
  Output:       .
  Return:       Status.
  Others:       None.
******************************************************************************/
STATUS ShutdownSocket(const S32 socketId, const S32 how)
{
	if (STATUS_OK != shutdown(socketId, how))
	{
		LOG_ERROR(MODULE_ID_SOCKET, "Fail to shutdown the socket (socket ID: %d)! (errNo = [-%d][%s])\n", socketId, errno, strerror(errno));
		return STATUS_ERR;
	}

	LOG_INFO(MODULE_ID_SOCKET, "Shutdown the socket (socket ID: %d) by action (%d) successfully.\n", socketId, how);

	return STATUS_OK;
}

/******************************************************************************
  Function:     SetFlagToSocket()
  Description:  Set flag to the socket.
  Input:        .
  Output:       .
  Return:       Status.
  Others:       None.
******************************************************************************/
STATUS SetFlagToSocket(const S32 socketId, const U32 flag)
{
	S32 flags;

	/* Get flags from the socket. */
	if (STATUS_ERR == (flags = fcntl(socketId, F_GETFL, 0)))
	{
		LOG_ERROR(MODULE_ID_SOCKET, "Fail to get flags from the socket! (errNo = [-%d][%s])\n", errno, strerror(errno));
		return STATUS_ERR;
	}

	/* Set flag to the socket. */
	if (STATUS_ERR == fcntl(socketId, F_SETFL, (flags | flag)))
	{
		LOG_ERROR(MODULE_ID_SOCKET, "Fail to set flags to the socket! (errNo = [-%d][%s])\n", errno, strerror(errno));
		return STATUS_ERR;
	}

	LOG_INFO(MODULE_ID_SOCKET, "Set a flag (%#X) to the socket (socket ID: %d) successfully.\n", flag, socketId);

	return STATUS_OK;
}

/******************************************************************************
  Function:     ConfigSocketWithBufLen()
  Description:  .
  Input:        .
  Output:       .
  Return:       Status.
  Others:       None.
******************************************************************************/
STATUS ConfigSocketWithBufLen(const S32 socketId, const U32 rcvBufLen, const U32 sndBufLen)
{
    U32 actRcvBufLen, actSndBufLen;
    socklen_t valLen = sizeof(U32);

	/* Set the size of the socket-level receive buffer to be allocated. */
	if (rcvBufLen > 0)
	{
		if (STATUS_OK != setsockopt(socketId, SOL_SOCKET, SO_RCVBUF, &rcvBufLen, sizeof(U32)))
		{
			LOG_ERROR(MODULE_ID_SOCKET, "Fail to set the size of the socket-level receive buffer to be allocated!\n");
			return STATUS_ERR;
		}
	}

	/* Get and print the size of the socket-level receive buffer. */
	if (STATUS_OK != getsockopt(socketId, SOL_SOCKET, SO_RCVBUF, &actRcvBufLen, &valLen))
	{
		LOG_WARN(MODULE_ID_SOCKET, "Fail to get the size of the socket-level receive buffer!\n");
	}
	else
	{
		LOG_DBG(MODULE_ID_SOCKET, "The size of the socket-level receive buffer: %u bytes.\n", actRcvBufLen);
	}

	/* Set the size of the socket-level send buffer to be allocated. */
	if (sndBufLen > 0)
	{
		if (STATUS_OK != setsockopt(socketId, SOL_SOCKET, SO_SNDBUF, &sndBufLen, sizeof(U32)))
		{
			LOG_ERROR(MODULE_ID_SOCKET, "Fail to set the size of the socket-level send buffer to be allocated!\n");
			return STATUS_ERR;
		}
	}

	/* Get and print the size of the socket-level send buffer. */
	if (STATUS_OK != getsockopt(socketId, SOL_SOCKET, SO_SNDBUF, &actSndBufLen, &valLen))
	{
		LOG_WARN(MODULE_ID_SOCKET, "Fail to get the size of the socket-level send buffer!\n");
	}
	else
	{
		LOG_DBG(MODULE_ID_SOCKET, "The size of the socket-level send buffer: %u bytes.\n", actSndBufLen);
	}

	LOG_INFO(MODULE_ID_SOCKET, "Configure the socket (socket ID: %d) with buffer length successfully.\n", socketId);

	return STATUS_OK;
}

/******************************************************************************
  Function:     ConfigSocketAddrReusable()
  Description:  .
  Input:        .
  Output:       .
  Return:       Status.
  Others:       None.
******************************************************************************/
STATUS ConfigSocketAddrReusable(const S32 socketId)
{
	S32 flag = 1;

	if (STATUS_OK != setsockopt(socketId, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(S32)))
	{
		LOG_ERROR(MODULE_ID_SOCKET, "Fail to configure the address of the socket reusable! (errNo = [-%d][%s])\n", errno, strerror(errno));
		return STATUS_ERR;
	}

	LOG_INFO(MODULE_ID_SOCKET, "Configure the address of the socket (socket ID: %d) reusable successfully.\n", socketId);

	return STATUS_OK;
}

/******************************************************************************
  Function:     ConfigSocketWithAddrAndPort()
  Description:  .
  Input:        .
  Output:       .
  Return:       Status.
  Others:       None.
******************************************************************************/
STATUS ConfigSocketWithAddrAndPort(const S32 socketId, const U32 ipVer, const char* const pIpAddr, const U16 portNumber)
{
	void* pAddr;
	in_port_t* pPort;
	struct sockaddr_storage sAddr;
	char addrStr[INET6_ADDRSTRLEN];

	if (NULL == pIpAddr)
	{
		LOG_ERROR(MODULE_ID_SOCKET, "Pointer to ipAddr is NULL!\n");
		return STATUS_ERR;
	}

	/* Populate local IP address and local port number. */
	if (IP_ADDR_VER_4 == ipVer)
	{
		sAddr.ss_family = AF_INET;
		pAddr = (void*)&((struct sockaddr_in*)&sAddr)->sin_addr;
		pPort = &((struct sockaddr_in*)&sAddr)->sin_port;
	}
	else if (IP_ADDR_VER_6 == ipVer)
	{
		sAddr.ss_family = AF_INET6;
		pAddr = (void*)&((struct sockaddr_in6*)&sAddr)->sin6_addr;
		pPort = &((struct sockaddr_in6*)&sAddr)->sin6_port;
	}
	else
	{
		LOG_ERROR(MODULE_ID_SOCKET, "IP Version (%u) unidentified!\n", ipVer);
		return STATUS_ERR;
	}

	if (1 != inet_pton(sAddr.ss_family, pIpAddr, pAddr))
	{
		LOG_ERROR(MODULE_ID_SOCKET, "Fail to convert a presentation format address to a network format address! (errNo = [-%d][%s])\n", errno, strerror(errno));
		return STATUS_ERR;
	}

	*pPort = htons(portNumber);

	LOG_INFO(MODULE_ID_SOCKET, "Local IP Address: %s, Local Port Number: %u.\n", inet_ntop(sAddr.ss_family, pAddr, addrStr, INET6_ADDRSTRLEN), ntohs(*pPort));

	/* Bind local IP address and local port number to the socket. */
	if (STATUS_OK != bind(socketId, (struct sockaddr*)&sAddr, sizeof(struct sockaddr_storage)))
	{
		LOG_ERROR(MODULE_ID_SOCKET, "Fail to bind the socket with IP Address and Port Number! (errNo = [-%d][%s])\n", errno, strerror(errno));
		return STATUS_ERR;
	}

	LOG_INFO(MODULE_ID_SOCKET, "Configure the socket (socket ID: %d) with address and port successfully.\n", socketId);

	return STATUS_OK;
}

/******************************************************************************
  Function:     ConfigSocketWithTos()
  Description:  Config UDP/IPv4 socket with TOS field.
  Input:        .
  Output:       .
  Return:       Status.
  Others:       None.
******************************************************************************/
STATUS ConfigSocketWithTos(const S32 socketId, const U8 tos)
{
	LOG_INFO(MODULE_ID_SOCKET, "TOS: %#X.\n", tos);

	if (STATUS_OK != setsockopt(socketId, IPPROTO_IP, IP_TOS, &tos, sizeof(U8)))
	{
		LOG_ERROR(MODULE_ID_SOCKET, "Fail to set TOS of the socket! (errNo = [-%d][%s])\n", errno, strerror(errno));
		return STATUS_ERR;
	}

	LOG_INFO(MODULE_ID_SOCKET, "Configure the socket (socket ID: %d) with TOS (%u) successfully.\n", socketId, tos);

	return STATUS_OK;
}

/******************************************************************************
  Function:     ListenToTcpClients()
  Description:  Transfer to Listen state on TCP server.
  Input:        .
  Output:       .
  Return:       Status.
  Others:       None.
******************************************************************************/
STATUS ListenToTcpClients(const S32 socketId, const S32 maxNumConns)
{
	if (STATUS_OK != listen(socketId, maxNumConns))
	{
		LOG_ERROR(MODULE_ID_SOCKET, "Fail to transfer to Listen state on TCP server! (errNo = [-%d][%s])\n", errno, strerror(errno));
		return STATUS_ERR;
	}

	return STATUS_OK;
}

/******************************************************************************
  Function:     ConnectTcpServer()
  Description:  .
  Input:        .
  Output:       .
  Return:       Status.
  Others:       None.
******************************************************************************/
STATUS ConnectTcpServer(const S32 socketId, const U32 ipVer, const char* const pIpAddr, const U16 portNumber)
{
	void* pAddr;
	in_port_t* pPort;
	struct sockaddr_storage sAddr;
	char addrStr[INET6_ADDRSTRLEN];

	if (NULL == pIpAddr)
	{
		LOG_ERROR(MODULE_ID_SOCKET, "Pointer to ipAddr is NULL!\n");
		return STATUS_ERR;
	}

	/* Populate romote IP address and remote port number. */
	if (IP_ADDR_VER_4 == ipVer)
	{
		sAddr.ss_family = AF_INET;
		pAddr = (void*)&((struct sockaddr_in*)&sAddr)->sin_addr;
		pPort = &((struct sockaddr_in*)&sAddr)->sin_port;
	}
	else if (IP_ADDR_VER_6 == ipVer)
	{
		sAddr.ss_family = AF_INET6;
		pAddr = (void*)&((struct sockaddr_in6*)&sAddr)->sin6_addr;
		pPort = &((struct sockaddr_in6*)&sAddr)->sin6_port;
	}
	else
	{
		LOG_ERROR(MODULE_ID_SOCKET, "IP Version (%u) unidentified!\n", ipVer);
		return STATUS_ERR;
	}

	if (1 != inet_pton(sAddr.ss_family, pIpAddr, pAddr))
	{
		LOG_ERROR(MODULE_ID_SOCKET, "Fail to convert a presentation format address to a network format address! (errNo = [-%d][%s])\n", errno, strerror(errno));
		return STATUS_ERR;
	}

	*pPort = htons(portNumber);

	LOG_INFO(MODULE_ID_SOCKET, "Remote IP Address: %s, Remote Port Number: %u.\n", inet_ntop(sAddr.ss_family, pAddr, addrStr, INET6_ADDRSTRLEN), ntohs(*pPort));

	/* Connect the TCP server. */
	if (STATUS_OK != connect(socketId, (struct sockaddr*)&sAddr, sizeof(struct sockaddr_storage)))
	{
		LOG_ERROR(MODULE_ID_SOCKET, "Fail to connect the TCP server! (errNo = [-%d][%s])\n", errno, strerror(errno));
		return STATUS_ERR;
	}

	return STATUS_OK;
}

/******************************************************************************
  Function:     AcceptTcpClient()
  Description:  .
  Input:        .
  Output:       .
  Return:       Connection Socket ID.
  Others:       None.
******************************************************************************/
S32 AcceptTcpClient(const S32 listenSockId, S_IpAddr* const pIpAddr, U16* const pPortNumber)
{
	S32 connSockId;
	void* pAddr;
	in_port_t* pPort;
	struct sockaddr_storage sAddr;
	socklen_t addrLen = sizeof(struct sockaddr_storage);
	char addrStr[INET6_ADDRSTRLEN];

	if (NULL == pIpAddr)
	{
		LOG_WARN(MODULE_ID_SOCKET, "Pointer to ipAddr is NULL!\n");
		return STATUS_ERR;
	}

	if (NULL == pPortNumber)
	{
		LOG_WARN(MODULE_ID_SOCKET, "Pointer to portNumber is NULL!\n");
		return STATUS_ERR;
	}

	/* Accept TCP client. */
	connSockId = accept(listenSockId, (struct sockaddr*)&sAddr, &addrLen);

	if (connSockId >= 0)
	{
		/* Populate remote IP address and remote port number. */
		if (AF_INET == sAddr.ss_family)
		{
			pIpAddr->ver = IP_ADDR_VER_4;
			pAddr = (void*)&((struct sockaddr_in*)&sAddr)->sin_addr.s_addr;
			pPort = &((struct sockaddr_in*)&sAddr)->sin_port;
		}
		else if (AF_INET6 == sAddr.ss_family)
		{
			pIpAddr->ver = IP_ADDR_VER_6;
			pAddr = (void*)((struct sockaddr_in6*)&sAddr)->sin6_addr.s6_addr;
			pPort = &((struct sockaddr_in6*)&sAddr)->sin6_port;
		}
		else
		{
			LOG_WARN(MODULE_ID_SOCKET, "Address Family (%u) unsupported!\n", sAddr.ss_family);
			return STATUS_ERR;
		}

		memcpy(pIpAddr->addr, pAddr, pIpAddr->ver);

		*pPortNumber = ntohs(*pPort);

		LOG_INFO(MODULE_ID_SOCKET, "A TCP client (socket ID: %d, IP Address: %s, Port Number: %u) accepted.\n", connSockId, inet_ntop(sAddr.ss_family, pAddr, addrStr, INET6_ADDRSTRLEN), ntohs(*pPort));
	}

	return connSockId;
}

/******************************************************************************
  Function:     IsTcpSocketConnected()
  Description:  .
  Input:        .
  Output:       true - connected, false - disconnected.
  Return:       Status.
  Others:       None.
******************************************************************************/
STATUS IsTcpSocketConnected(const S32 socketId, bool* const pConnected)
{
	if (NULL == pConnected)
	{
		LOG_WARN(MODULE_ID_SOCKET, "Pointer to connected is NULL!\n");
		return STATUS_ERR;
	}

	struct tcp_info info;
    socklen_t valLen = sizeof(struct tcp_info);

	/* Get the TCP info of the socket. */
	if (STATUS_OK != getsockopt(socketId, IPPROTO_TCP, TCP_INFO, &info, &valLen))
	{
		LOG_WARN(MODULE_ID_SOCKET, "Fail to get the TCP info of the socket!\n");
		return STATUS_ERR;
	}

	if (TCP_ESTABLISHED == info.tcpi_state)
	{
		*pConnected = true;
	}
	else
	{
		*pConnected = false;
	}

	return STATUS_OK;
}

/******************************************************************************
  Function:     RecvFromTcpSocket()
  Description:  .
  Input:        .
  Output:       .
  Return:       The number of bytes received, or ERROR if the call fails.
  Others:       None.
******************************************************************************/
S32 RecvFromTcpSocket(const S32 socketId, void* const pBuffer, const U32 bufLen, const S32 flags)
{
	if (NULL == pBuffer)
	{
		LOG_WARN(MODULE_ID_SOCKET, "Pointer to buffer is NULL!\n");
		return STATUS_ERR;
	}

	/* Receive data from the TCP socket. */
	return recv(socketId, pBuffer, bufLen, flags);
}

/******************************************************************************
  Function:     RecvFromUdpSocket()
  Description:  .
  Input:        .
  Output:       .
  Return:       The number of bytes received, or ERROR if the call fails.
  Others:       None.
******************************************************************************/
S32 RecvFromUdpSocket(const S32 socketId, void* const pBuffer, const U32 bufLen,
		const S32 flags, S_IpAddr* const pIpAddr, U16* const pPortNumber)
{
	S32 len;
	void* pAddr;
	in_port_t* pPort;
	struct sockaddr_storage sAddr;
	socklen_t addrLen = sizeof(struct sockaddr_storage);

	if (NULL == pBuffer)
	{
		LOG_WARN(MODULE_ID_SOCKET, "Pointer to buffer is NULL!\n");
		return STATUS_ERR;
	}

	if (NULL == pIpAddr)
	{
		LOG_WARN(MODULE_ID_SOCKET, "Pointer to ipAddr is NULL!\n");
		return STATUS_ERR;
	}

	if (NULL == pPortNumber)
	{
		LOG_WARN(MODULE_ID_SOCKET, "Pointer to portNumber is NULL!\n");
		return STATUS_ERR;
	}

	/* Receive data from the UDP socket. */
	len = recvfrom(socketId, pBuffer, bufLen, flags, (struct sockaddr*)&sAddr, &addrLen);

	/* Populate source IP address and source port number. */
	if (len > 0)
	{
		if (AF_INET == sAddr.ss_family)
		{
			pIpAddr->ver = IP_ADDR_VER_4;
			pAddr = (void*)&((struct sockaddr_in*)&sAddr)->sin_addr.s_addr;
			pPort = &((struct sockaddr_in*)&sAddr)->sin_port;
		}
		else if (AF_INET6 == sAddr.ss_family)
		{
			pIpAddr->ver = IP_ADDR_VER_6;
			pAddr = (void*)((struct sockaddr_in6*)&sAddr)->sin6_addr.s6_addr;
			pPort = &((struct sockaddr_in6*)&sAddr)->sin6_port;
		}
		else
		{
			LOG_WARN(MODULE_ID_SOCKET, "Address Family (%u) unsupported!\n", sAddr.ss_family);
			return STATUS_ERR;
		}

		memcpy(pIpAddr->addr, pAddr, pIpAddr->ver);

		*pPortNumber = ntohs(*pPort);
	}

	return len;
}

/******************************************************************************
  Function:     SendToTcpSocket()
  Description:  .
  Input:        .
  Output:       .
  Return:       The number of bytes sent, or ERROR if the call fails.
  Others:       None.
******************************************************************************/
S32 SendToTcpSocket(const S32 socketId, const void* const pData, const U32 dataLen, const S32 flags)
{
	S32 len;

	if (NULL == pData)
	{
		LOG_WARN(MODULE_ID_SOCKET, "Pointer to data is NULL!\n");
		return STATUS_ERR;
	}

	/* Send the data to the TCP socket. */
	len = send(socketId, pData, dataLen, flags);

	if ((S32)dataLen != len)
	{
		LOG_WARN(MODULE_ID_SOCKET, "Failed or incompleted sending the data to the TCP socket! (errNo = [-%d][%s])\n", errno, strerror(errno));
	}

	return len;
}

/******************************************************************************
  Function:     SendToUdpSocket()
  Description:  .
  Input:        .
  Output:       .
  Return:       The number of bytes sent, or ERROR if the call fails.
  Others:       None.
******************************************************************************/
S32 SendToUdpSocket(const S32 socketId, const void* const pData, const U32 dataLen,
		const S32 flags, const S_IpAddr* const pIpAddr, const U16 portNumber)
{
	S32 len;
	void* pAddr;
	in_port_t* pPort;
	struct sockaddr_storage sAddr;

	if (NULL == pData)
	{
		LOG_WARN(MODULE_ID_SOCKET, "Pointer to data is NULL!\n");
		return STATUS_ERR;
	}

	if (NULL == pIpAddr)
	{
		LOG_WARN(MODULE_ID_SOCKET, "Pointer to ipAddr is NULL!\n");
		return STATUS_ERR;
	}

	/* Populate destination IP address and destination port number. */
	if (IP_ADDR_VER_4 == pIpAddr->ver)
	{
		sAddr.ss_family = AF_INET;
		pAddr = (void*)&((struct sockaddr_in*)&sAddr)->sin_addr.s_addr;
		pPort = &((struct sockaddr_in*)&sAddr)->sin_port;
	}
	else if (IP_ADDR_VER_6 == pIpAddr->ver)
	{
		sAddr.ss_family = AF_INET6;
		pAddr = (void*)((struct sockaddr_in6*)&sAddr)->sin6_addr.s6_addr;
		pPort = &((struct sockaddr_in6*)&sAddr)->sin6_port;
	}
	else
	{
		LOG_WARN(MODULE_ID_SOCKET, "IP Version (%u) unidentified!\n", pIpAddr->ver);
		return STATUS_ERR;
	}

	memcpy(pAddr, pIpAddr->addr, pIpAddr->ver);

	*pPort = htons(portNumber);

	/* Send the data to the UDP socket. */
	len = sendto(socketId, pData, dataLen, flags, (struct sockaddr*)&sAddr, sizeof(struct sockaddr_storage));

	if ((S32)dataLen != len)
	{
		LOG_WARN(MODULE_ID_SOCKET, "Failed or incompleted sending the data to the UDP socket! (errNo = [-%d][%s])\n", errno, strerror(errno));
	}

	return len;
}

/* End of File */

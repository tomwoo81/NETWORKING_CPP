#pragma once

#include "SocketIf.h"

class TcpSocketIf: public SocketIf
{
public:
	TcpSocketIf();
	virtual ~TcpSocketIf();

#if 0
	STATUS isConnected(bool& connected);
#endif
	S32 recv(void* const pBuffer, const U32 bufLen, const S32 flags);
	S32 send(const void* const pData, const U32 dataLen, const S32 flags);

protected:
	S_IpAddr msRemoteIpAddr;
	std::string mRemoteIpAddrStr;
	U16 mRemotePortNumber;
};

class TcpClientSocket: public TcpSocketIf
{
public:
	TcpClientSocket();
	virtual ~TcpClientSocket();

	STATUS connect(const U32 ipVer, const std::string& ipAddrStr, const U16 portNumber);
};

class TcpServerSocket: public TcpSocketIf
{
public:
	TcpServerSocket();
	virtual ~TcpServerSocket();

	STATUS listen(const S32 maxNumConns);
	S32 accept(S_IpAddr& sIpAddr, U16& portNumber, TcpServerSocket& connTcpServerSocket);
};

/* End of File */

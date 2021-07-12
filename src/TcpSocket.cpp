#include <arpa/inet.h>
#include <sys/socket.h>
#include "SocketAdapter.h"
#include "TcpSocket.h"

TcpSocketIf::TcpSocketIf()
: mRemotePortNumber(0)
{
	mSocketType = SOCK_STREAM;
	msRemoteIpAddr.ver = 0;
}

TcpSocketIf::~TcpSocketIf()
{
}

STATUS TcpSocketIf::isConnected(bool& connected)
{
	return IsTcpSocketConnected(mSocketId, &connected);
}

S32 TcpSocketIf::recv(void* const pBuffer, const U32 bufLen, const S32 flags)
{
	return RecvFromTcpSocket(mSocketId, pBuffer, bufLen, flags);
}

S32 TcpSocketIf::send(const void* const pData, const U32 dataLen, const S32 flags)
{
	return SendToTcpSocket(mSocketId, pData, dataLen, flags);
}

TcpClientSocket::TcpClientSocket()
{
}

TcpClientSocket::~TcpClientSocket()
{
}

STATUS TcpClientSocket::connect(const U32 ipVer, const std::string& ipAddrStr, const U16 portNumber)
{
	S32 ret;

	ret = ConnectTcpServer(mSocketId, ipVer, ipAddrStr.c_str(), portNumber);
	if (STATUS_ERR == ret)
	{
		return STATUS_ERR;
	}

	inet_pton((IP_ADDR_VER_4 == ipVer) ? AF_INET : AF_INET6, ipAddrStr.c_str(), msRemoteIpAddr.addr);
	msRemoteIpAddr.ver = ipVer;

	mRemoteIpAddrStr = ipAddrStr;

	mRemotePortNumber = portNumber;

	return STATUS_OK;
}

TcpServerSocket::TcpServerSocket()
{
}

TcpServerSocket::~TcpServerSocket()
{
}

STATUS TcpServerSocket::listen(const S32 maxNumConnReqsInQue)
{
	return ListenToTcpClients(mSocketId, maxNumConnReqsInQue);
}

S32 TcpServerSocket::accept(S_IpAddr& sIpAddr, U16& portNumber, TcpServerSocket& connTcpServerSocket)
{
	S32 ret;
	char addrStr[INET6_ADDRSTRLEN];

	ret = AcceptTcpClient(mSocketId, &sIpAddr, &portNumber);
	if (STATUS_ERR == ret)
	{
		return STATUS_ERR;
	}

	connTcpServerSocket.mSocketId = ret;

	connTcpServerSocket.msLocalIpAddr = msLocalIpAddr;
	connTcpServerSocket.mLocalIpAddrStr = mLocalIpAddrStr;
	connTcpServerSocket.mLocalPortNumber = mLocalPortNumber;

	connTcpServerSocket.msRemoteIpAddr = sIpAddr;
	inet_ntop((IP_ADDR_VER_4 == sIpAddr.ver) ? AF_INET : AF_INET6, sIpAddr.addr, addrStr, INET6_ADDRSTRLEN);
	connTcpServerSocket.mRemoteIpAddrStr = addrStr;
	connTcpServerSocket.mRemotePortNumber = portNumber;

	return ret;
}

/* End of File */

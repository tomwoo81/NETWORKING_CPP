#include <sys/socket.h>
#include "SocketAdapter.h"
#include "UdpSocket.h"

UdpSocket::UdpSocket()
{
	mSocketType = SOCK_DGRAM;
}

UdpSocket::~UdpSocket()
{
}

S32 UdpSocket::recv(void* const pBuffer, const U32 bufLen, const S32 flags, S_IpAddr& sIpAddr, U16& portNumber)
{
	return RecvFromUdpSocket(mSocketId, pBuffer, bufLen, flags, &sIpAddr, &portNumber);
}

S32 UdpSocket::send(const void* const pData, const U32 dataLen, const S32 flags, const S_IpAddr& sIpAddr, const U16 portNumber)
{
	return SendToUdpSocket(mSocketId, pData, dataLen, flags, &sIpAddr, portNumber);
}

/* End of File */

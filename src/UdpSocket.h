#pragma once

#include "SocketIf.h"

class UdpSocket: public SocketIf
{
public:
	UdpSocket();
	virtual ~UdpSocket();

	S32 recv(void* const pBuffer, const U32 bufLen,
			const S32 flags, S_IpAddr& sIpAddr, U16& portNumber);
	S32 send(const void* const pData, const U32 dataLen,
			const S32 flags, const S_IpAddr& sIpAddr, const U16 portNumber);
};

/* End of File */

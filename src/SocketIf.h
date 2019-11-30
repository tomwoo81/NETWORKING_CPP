#pragma once

#include <string>
#include "CommonTypes.h"
#include "IpAddr.h"

class SocketIf
{
public:
	SocketIf();
	virtual ~SocketIf();

	S32 openAndConfig(const U32 ipVer, const U32 flag,
			const U32 rcvBufLen, const U32 sndBufLen,
			const std::string& ipAddrStr, const U16 portNumber);
	STATUS close();
	STATUS shutdown(const S32 how);
	void reset();
	STATUS setFlag(const U32 flag);
	STATUS configWithBufLen(const U32 rcvBufLen, const U32 sndBufLen);
	STATUS configWithAddrAndPort(const U32 ipVer, const std::string& ipAddrStr, const U16 portNumber);
	STATUS configWithTos(const U8 tos);

protected:
	S32 mSocketId;
	S32 mSocketType;
	U32 mRxBufLen;
	U32 mTxBufLen;
	S_IpAddr msLocalIpAddr;
	std::string mLocalIpAddrStr;
	U16 mLocalPortNumber;
	U8 mTos;
};

/* End of File */

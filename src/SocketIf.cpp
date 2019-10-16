#include <arpa/inet.h>
#include <sys/socket.h>
#include "SocketAdapter.h"
#include "SocketIf.h"

SocketIf::SocketIf()
: mSocketId(-1), mSocketType(0), mRxBufLen(0), mTxBufLen(0), mLocalPortNumber(0), mTos(0)
{
	msLocalIpAddr.ver = 0;
}

SocketIf::~SocketIf()
{
	if (-1 != mSocketId)
	{
		close();
	}
}

S32 SocketIf::openAndConfig(const U32 ipVer, const U32 flag, const U32 rcvBufLen,
			const U32 sndBufLen, const std::string& ipAddrStr, const U16 portNumber)
{
	S32 ret;

	ret = OpenAndConfigSocket(ipVer, mSocketType, flag, rcvBufLen, sndBufLen, ipAddrStr.c_str(), portNumber);
	if (STATUS_ERR == ret)
	{
		return STATUS_ERR;
	}

	if (ipAddrStr.length() > 0)
	{
		inet_pton((IP_ADDR_VER_4 == ipVer) ? AF_INET : AF_INET6, ipAddrStr.c_str(), msLocalIpAddr.addr);
		msLocalIpAddr.ver = ipVer;

		mLocalIpAddrStr = ipAddrStr;

		mLocalPortNumber = portNumber;
	}

	mSocketId = ret;
	mRxBufLen = rcvBufLen;
	mTxBufLen = sndBufLen;

	return ret;
}

STATUS SocketIf::close()
{
	S32 ret;

	ret = CloseSocket(mSocketId);
	if (STATUS_ERR == ret)
	{
		return STATUS_ERR;
	}

	mSocketId = -1;
	mRxBufLen = 0;
	mTxBufLen = 0;
	msLocalIpAddr.ver = 0;
	mLocalIpAddrStr.clear();
	mLocalPortNumber = 0;
	mTos = 0;

	return STATUS_OK;
}

STATUS SocketIf::shutdown(const S32 how)
{
	return ShutdownSocket(mSocketId, how);
}

void SocketIf::reset()
{
	mSocketId = -1;
	mRxBufLen = 0;
	mTxBufLen = 0;
	msLocalIpAddr.ver = 0;
	mLocalIpAddrStr.clear();
	mLocalPortNumber = 0;
	mTos = 0;
}

STATUS SocketIf::setFlag(const U32 flag)
{
	return SetFlagToSocket(mSocketId, flag);
}

STATUS SocketIf::configWithAddrAndPort(const U32 ipVer, const std::string& ipAddrStr, const U16 portNumber)
{
	S32 ret;

	ret = ConfigSocketWithAddrAndPort(mSocketId, ipVer, ipAddrStr.c_str(), portNumber);
	if (STATUS_ERR == ret)
	{
		return STATUS_ERR;
	}

	inet_pton((IP_ADDR_VER_4 == ipVer) ? AF_INET : AF_INET6, ipAddrStr.c_str(), msLocalIpAddr.addr);
	msLocalIpAddr.ver = ipVer;

	mLocalIpAddrStr = ipAddrStr;

	mLocalPortNumber = portNumber;

	return STATUS_OK;
}

STATUS SocketIf::configWithTos(const U8 tos)
{
	S32 ret;

	ret = ConfigSocketWithTos(mSocketId, tos);
	if (STATUS_ERR == ret)
	{
		return STATUS_ERR;
	}

	mTos = tos;

	return STATUS_OK;
}

/* End of File */

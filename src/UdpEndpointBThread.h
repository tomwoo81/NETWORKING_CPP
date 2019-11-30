#pragma once

#include "ThreadIf.h"

class UdpEndpointBThread: public ThreadIf
{
public:
	UdpEndpointBThread(const U32 ipVer, const std::string& localIpAddrStr, const U16 localPortNumber,
			const std::string& remoteIpAddrStr, const U16 remotePortNumber);
	virtual ~UdpEndpointBThread();

	virtual void start();
	virtual void run();

private:
	U32 mIpVer;
	std::string mLocalIpAddrStr;
	U16 mLocalPortNumber;
	std::string mRemoteIpAddrStr;
	U16 mRemotePortNumber;
};

/* End of File */

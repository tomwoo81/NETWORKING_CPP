#pragma once

#include <mutex>
#include <condition_variable>
#include "ThreadIf.h"

class UdpEndpointAThread: public ThreadIf
{
public:
	UdpEndpointAThread(const U32 ipVer, const std::string& localIpAddrStr, const U16 localPortNumber);
	virtual ~UdpEndpointAThread();

	virtual void start();
	virtual void run();
	void shutdown();
	bool isShutdown() const;

private:
	bool waitForShutdown(const S64 ms) const;

private:
	U32 mIpVer;
	std::string mLocalIpAddrStr;
	U16 mLocalPortNumber;
	bool mShutdown;
	mutable std::mutex mMutex;
	mutable std::condition_variable mCondition;
};

/* End of File */

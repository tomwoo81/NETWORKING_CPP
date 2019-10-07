#pragma once

#include "ThreadIf.h"
#include "TcpSocket.h"
#include "ThreadPool.h"

class TcpServerListenThread: public ThreadIf
{
public:
	TcpServerListenThread(const U32 ipVer, const std::string& localIpAddrStr, const U16 localPortNumber);
	virtual ~TcpServerListenThread();

	virtual void start();
	virtual void run();

private:
	U32 mIpVer;
	std::string mLocalIpAddrStr;
	U16 mLocalPortNumber;
	TcpServerSocket mListenTcpServerSocket;
	ThreadPool* mpThreadPool;
};

/* End of File */

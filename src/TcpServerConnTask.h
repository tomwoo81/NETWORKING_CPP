#pragma once

#include "TcpSocket.h"
#include "ThreadPool.h"

class TcpServerConnTask: public PoolTaskIf
{
public:
	TcpServerConnTask(const TcpServerSocket& socket);
	virtual ~TcpServerConnTask();

	virtual void run();

private:
	TcpServerSocket mSocket;
};

/* End of File */

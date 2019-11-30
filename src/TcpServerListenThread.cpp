#include <iostream>
#include <fcntl.h>
//#include "rutil/Logger.hxx"
#include "MacroDefs.h"
#include "TcpSocket.h"
#include "TcpServerConnTask.h"
#include "TcpServerListenThread.h"

#define TCP_SERVER_THREAD_BUFFER_LENGTH (1 * 1024 * 1024) // 1 MB

TcpServerListenThread::TcpServerListenThread(const U32 ipVer, const std::string& localIpAddrStr, const U16 localPortNumber)
: mIpVer(ipVer), mLocalIpAddrStr(localIpAddrStr), mLocalPortNumber(localPortNumber), mShutdown(false)
{
}

TcpServerListenThread::~TcpServerListenThread()
{
}

void TcpServerListenThread::start()
{
	try
	{
		mpThread = new std::thread(&TcpServerListenThread::run, this);
	}
	catch (const std::bad_alloc& e)
	{
		std::cout << "[Err] " << "Fail to create an instance of TcpServerListenThread!" << std::endl;
		return;
	}
}

void TcpServerListenThread::run()
{
//	InfoLog(<<"TcpServerListenThread start.");
	std::cout << "[Info] " << "TcpServerListenThread start." << std::endl;

	S32 ret;
	S_IpAddr sIpAddr;
	U16 portNumber;
	TcpServerSocket listenTcpServerSocket;
	TcpServerSocket connTcpServerSocket;
	TcpServerConnTask* pTask;

	ThreadPool threadPool(THREAD_POOL_NUM_THREADS);

	/* Open a TCP Server Socket for listening. */
	ret = listenTcpServerSocket.openAndConfig(mIpVer, O_NONBLOCK,
			TCP_SERVER_THREAD_BUFFER_LENGTH, TCP_SERVER_THREAD_BUFFER_LENGTH,
			"", 0); // Non-blocking I/O
	if (STATUS_ERR == ret)
	{
//		ErrLog(<<"Fail to open a TCP Server Socket for listening!");
		std::cout << "[Err] " << "Fail to open a TCP Server Socket for listening!" << std::endl;
		return;
	}

	/* Configure a TCP Server Socket for listening with addr and port. */
	ret = listenTcpServerSocket.configWithAddrAndPort(mIpVer, mLocalIpAddrStr, mLocalPortNumber);
	if (STATUS_ERR == ret)
	{
//		ErrLog(<<"Fail to configure a TCP Server Socket for listening with addr and port!");
		std::cout << "[Err] " << "Fail to configure a TCP Server Socket for listening with addr and port!" << std::endl;
		return;
	}

	/* Transfer to Listen state on TCP server. */
	ret = listenTcpServerSocket.listen(TCP_CONNECTIONS_MAX_NUM);
	if (STATUS_ERR == ret)
	{
//		ErrLog(<<"Fail to transfer to Listen state on TCP server!");
		std::cout << "[Err] " << "Fail to transfer to Listen state on TCP server!" << std::endl;
		return;
	}

	while (!waitForShutdown(10)) // Wait for 10 ms.
	{
		while (true)
		{
			/* Accept TCP client. */
			ret = listenTcpServerSocket.accept(sIpAddr, portNumber, connTcpServerSocket);
			if (STATUS_ERR == ret)
			{
				/* No new TCP connection. */
				break;
			}
			else
			{
				/* A new TCP connection. */
				try
				{
					/* Create a TCP Server Connection Task. */
					pTask = new TcpServerConnTask(connTcpServerSocket);
					connTcpServerSocket.reset();
				}
				catch (const std::bad_alloc& e)
				{
	//				ErrLog(<<"Fail to create an HTTP Server Connection Task!");
					std::cout << "[Err] " << "Fail to create an HTTP Server Connection Task!" << std::endl;
					connTcpServerSocket.close();
					continue;
				}

				/* Add a TCP Server Connection Task to the Thread Pool. */
				threadPool.addTask(pTask);
	//			InfoLog(<<"Add a TCP Server Connection Task to the Thread Pool.");
				std::cout << "[Info] " << "Add a TCP Server Connection Task to the Thread Pool." << std::endl;
			}
		}
	}

	/* Shutdown all threads in the Thread Pool. */
	threadPool.shutdownAll();
	threadPool.joinAll();

//	InfoLog(<<"TcpServerListenThread exit.");
	std::cout << "[Info] " << "TcpServerListenThread exit." << std::endl;
}

void TcpServerListenThread::shutdown()
{
	std::unique_lock<std::mutex> lock(mMutex);

	if (!mShutdown)
	{
		mShutdown = true;
		mCondition.notify_one();
	}
}

bool TcpServerListenThread::isShutdown() const
{
	std::unique_lock<std::mutex> lock(mMutex);

	return mShutdown;
}

bool TcpServerListenThread::waitForShutdown(const S64 ms) const
{
	std::unique_lock<std::mutex> lock(mMutex);

	if (!mShutdown)
	{
		mCondition.wait_for(lock, std::chrono::milliseconds(ms));
	}

	return mShutdown;
}

/* End of File */

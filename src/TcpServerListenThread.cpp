#include <iostream>
//#include "rutil/Logger.hxx"
#include "MacroDefs.h"
#include "TcpSocket.h"
#include "TcpServerConnTask.h"
#include "TcpServerListenThread.h"

#define TCP_SERVER_THREAD_BUFFER_LENGTH (1 * 1024 * 1024) //1 MB

void TcpServerListenThreadRun(TcpServerListenThread* const pThread)
{
	std::cout << "[Info] " << "TcpServerListenThread - enter" << std::endl;

	if (NULL == pThread)
	{
		std::cout << "[Err] " << "Pointer to the instance of TcpServerListenThread is NULL!" << std::endl;
		std::cout << "[Info] " << "TcpServerListenThread - exit" << std::endl;
		return;
	}

	pThread->run();

	std::cout << "[Info] " << "TcpServerListenThread - exit" << std::endl;
}

TcpServerListenThread::TcpServerListenThread(const U32 ipVer, const std::string& localIpAddrStr, const U16 localPortNumber)
: mIpVer(ipVer), mLocalIpAddrStr(localIpAddrStr), mLocalPortNumber(localPortNumber)
{
	try
	{
		mpThreadPool = new ThreadPool(THREAD_POOL_NUM_THREADS);
	}
	catch (const std::bad_alloc& e)
	{
//		ErrLog(<<"Fail to create a Thread Pool!");
		std::cout << "[Err] " << "Fail to create a Thread Pool!" << std::endl;
		return;
	}
}

TcpServerListenThread::~TcpServerListenThread()
{
	if (NULL != mpThreadPool)
	{
		delete mpThreadPool;
		mpThreadPool = NULL;
	}
}

void TcpServerListenThread::start()
{
	try
	{
		mpThread = new std::thread(TcpServerListenThreadRun, this);
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
//	std::cout << "[Info] " << "TcpServerListenThread start." << std::endl;

	S32 ret;
	S_IpAddr sIpAddr;
	U16 portNumber;
	TcpServerSocket connTcpServerSocket;
	TcpServerConnTask* pTask;

	/* Open a TCP Server Socket for listening. */
	ret = mListenTcpServerSocket.openAndConfig(mIpVer, 0,
			TCP_SERVER_THREAD_BUFFER_LENGTH, TCP_SERVER_THREAD_BUFFER_LENGTH,
			"", 0); //blocking I/O
	if (STATUS_ERR == ret)
	{
//		ErrLog(<<"Fail to open a TCP Server Socket for listening!");
		std::cout << "[Err] " << "Fail to open a TCP Server Socket for listening!" << std::endl;
		return;
	}

	/* Configure a TCP Server Socket for listening with addr and port. */
	ret = mListenTcpServerSocket.configWithAddrAndPort(mIpVer, mLocalIpAddrStr, mLocalPortNumber);
	if (STATUS_ERR == ret)
	{
//		ErrLog(<<"Fail to configure a TCP Server Socket for listening with addr and port!");
		std::cout << "[Err] " << "Fail to configure a TCP Server Socket for listening with addr and port!" << std::endl;
		return;
	}

	/* Transfer to Listen state on TCP server. */
	ret = mListenTcpServerSocket.listen(TCP_CONNECTIONS_MAX_NUM);
	if (STATUS_ERR == ret)
	{
//		ErrLog(<<"Fail to transfer to Listen state on TCP server!");
		std::cout << "[Err] " << "Fail to transfer to Listen state on TCP server!" << std::endl;
		return;
	}

	while (1)
	{
		/* Accept TCP client. */
		ret = mListenTcpServerSocket.accept(sIpAddr, portNumber, connTcpServerSocket);
		if (STATUS_ERR == ret)
		{
			/* No new TCP connection. */
			continue;
		}
		else
		{
			/* A new TCP connection. */
			try
			{
				/* Create a TCP Server Connection Task. */
				pTask = new TcpServerConnTask(connTcpServerSocket);
			}
			catch (const std::bad_alloc& e)
			{
//				ErrLog(<<"Fail to create an HTTP Server Connection Task!");
				std::cout << "[Err] " << "Fail to create an HTTP Server Connection Task!" << std::endl;
				connTcpServerSocket.close();
				continue;
			}

			/* Add a TCP Server Connection Task to the Thread Pool. */
			mpThreadPool->addTask(pTask);
//			InfoLog(<<"Add a TCP Server Connection Task to the Thread Pool.");
			std::cout << "[Info] " << "Add a TCP Server Connection Task to the Thread Pool." << std::endl;
		}
	}

//	InfoLog(<<"TcpServerListenThread exit.");
//	std::cout << "[Info] " << "TcpServerListenThread exit." << std::endl;
}

/* End of File */

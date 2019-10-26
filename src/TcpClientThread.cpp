#include <stdio.h>
#include <cstring>
#include <iostream>
//#include "rutil/Logger.hxx"
#include "MacroDefs.h"
#include "TcpSocket.h"
#include "TcpClientThread.h"

#define TCP_CLIENT_THREAD_BUFFER_LENGTH (1 * 1024 * 1024) //1 MB

TcpClientThread::TcpClientThread(const U32 ipVer, const std::string& localIpAddrStr, const U16 localPortNumber,
		const std::string& remoteIpAddrStr, const U16 remotePortNumber)
: mIpVer(ipVer), mLocalIpAddrStr(localIpAddrStr), mLocalPortNumber(localPortNumber),
  mRemoteIpAddrStr(remoteIpAddrStr), mRemotePortNumber(remotePortNumber)
{
}

TcpClientThread::~TcpClientThread()
{
}

void TcpClientThread::start()
{
	try
	{
		mpThread = new std::thread(&TcpClientThread::run, this);
	}
	catch (const std::bad_alloc& e)
	{
		std::cout << "[Err] " << "Fail to create an instance of TcpClientThread!" << std::endl;
		return;
	}
}

void TcpClientThread::run()
{
//	InfoLog(<<"TcpClientThread start.");
	std::cout << "[Info] " << "TcpClientThread start." << std::endl;

	S32 ret;
	TcpClientSocket socket;
	U8 rxBuffer[TCP_CLIENT_THREAD_BUFFER_LENGTH];
	std::string rxMsg;
	char txMsg[1024];

	/* Open and configure a TCP Client Socket with addr and port. */
	ret = socket.openAndConfig(mIpVer, 0,
			TCP_CLIENT_THREAD_BUFFER_LENGTH, TCP_CLIENT_THREAD_BUFFER_LENGTH,
			mLocalIpAddrStr, mLocalPortNumber); //blocking I/O
	if (STATUS_ERR == ret)
	{
//		ErrLog(<<"Fail to open and configure a TCP Client Socket with addr and port!");
		std::cout << "[Err] " << "Fail to open and configure a TCP Client Socket with addr and port!" << std::endl;
		return;
	}

	/* Connect the TCP server. */
	ret = socket.connect(mIpVer, mRemoteIpAddrStr, mRemotePortNumber);
	if (STATUS_ERR == ret)
	{
//		ErrLog(<<"Fail to connect the TCP server!");
		std::cout << "[Err] " << "Fail to connect the TCP server!" << std::endl;
		return;
	}

	for (U32 i = 0; i < TCP_CLIENT_TX_MSG_NUM; i++)
	{
		/* Send a message. */
		sprintf(txMsg, TCP_CLIENT_TX_MSG, i, mLocalPortNumber);
		ret = socket.send(txMsg, strlen(txMsg), 0);
		if (ret != (S32)strlen(txMsg))
		{
//			WarningLog(<<"Fail to send a message to the TCP server!");
			std::cout << "[Warn] " << "Fail to send a message to the TCP server!" << std::endl;
			continue;
		}
		std::cout << "[Info] " << "TCP Client (port: " << mLocalPortNumber << ") Tx (" << i << ") [" << ret << " bytes]" << std::endl;

		ret = socket.recv(rxBuffer, TCP_CLIENT_THREAD_BUFFER_LENGTH, 0);
		if (ret <= 0)
		{
			/* No received message. */
			break;
		}
		else
		{
			/* A received message. */

			rxMsg.assign((char*)rxBuffer, ret);
			std::cout << "[Info] " << "TCP Client (port: " << mLocalPortNumber << ") Rx (" << i << ") [" << ret << " bytes]" << std::endl;
			std::cout << "[Info] " << "TCP Client (port: " << mLocalPortNumber << ") Rx (" << i << "): " << rxMsg << std::endl;
		}

		std::this_thread::sleep_for(std::chrono::seconds(TCP_CLIENT_TX_MSG_INTERVAL));
	}

//	InfoLog(<<"TcpClientThread exit.");
	std::cout << "[Info] " << "TcpClientThread exit." << std::endl;
}

/* End of File */

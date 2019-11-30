#include <iostream>
#include <fcntl.h>
//#include "rutil/Logger.hxx"
#include "MacroDefs.h"
#include "TcpServerConnTask.h"

#define TCP_SERVER_TASK_BUFFER_LENGTH (512 * 1024) // 512 KB

TcpServerConnTask::TcpServerConnTask(const TcpServerSocket& socket)
: mSocket(socket)
{
}

TcpServerConnTask::~TcpServerConnTask()
{
}

void TcpServerConnTask::run()
{
	std::cout << "[Info] " << "TcpServerConnTask - enter" << std::endl;

	bool connected;
	S32 ret;
	U8 rxBuffer[TCP_SERVER_TASK_BUFFER_LENGTH];
	std::string rxMsg, txMsg;

	mSocket.setFlag(O_NONBLOCK); // Non-blocking I/O

	while (!mpThreadPool->isShutdown())
	{
		mSleep(10); // Wait for 10 ms.

		/* Check whether the TCP connection exists. */
		ret = mSocket.isConnected(connected);
		if ((STATUS_OK == ret) && !connected)
		{
//			InfoLog(<<"A TCP connection is shutdown.");
			std::cout << "[Info] " << "A TCP connection is shutdown." << std::endl;
			break;
		}

		while (true)
		{
			ret = mSocket.recv(rxBuffer, TCP_SERVER_TASK_BUFFER_LENGTH, 0);
			if (ret <= 0)
			{
				/* No received message. */
				break;
			}
			else
			{
				/* A received message. */

				rxMsg.assign((char*)rxBuffer, ret);
				std::cout << "[Info] " << "TCP Server Rx [" << std::dec << ret << " bytes]" << std::endl;
				std::cout << "[Info] " << "TCP Server Rx: " << rxMsg << std::endl;

				/* Send a message. */
				txMsg.assign(TCP_SERVER_TX_MSG);
				ret = mSocket.send(txMsg.c_str(), txMsg.length(), 0);
				if (ret != (S32)txMsg.length())
				{
	//				WarningLog(<<"Fail to send a message to a TCP client!");
					std::cout << "[Warn] " << "Fail to send a message to a TCP client!" << std::endl;
					continue;
				}
				std::cout << "[Info] " << "TCP Server Tx [" << std::dec << ret << " bytes]" << std::endl;
			}
		}
	}

	std::cout << "[Info] " << "TcpServerConnTask - exit" << std::endl;
}

/* End of File */

#include <iostream>
//#include "rutil/Logger.hxx"
#include "MacroDefs.h"
#include "TcpServerConnTask.h"

#define TCP_SERVER_TASK_BUFFER_LENGTH (512 * 1024) //512 KB

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

	S32 ret;
	U8 rxBuffer[TCP_SERVER_TASK_BUFFER_LENGTH];
	std::string rxMsg, txMsg;

	mSocket.setFlag(0); //blocking I/O

	while (1)
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
			std::cout << "[Info] " << "TCP Server Rx [" << ret << " bytes]" << std::endl;
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
			std::cout << "[Info] " << "TCP Server Tx [" << ret << " bytes]" << std::endl;
		}
	}

	std::cout << "[Info] " << "TcpServerConnTask - exit" << std::endl;
}

/* End of File */

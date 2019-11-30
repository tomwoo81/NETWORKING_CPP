#include <iostream>
#include <fcntl.h>
//#include "rutil/Logger.hxx"
#include "MacroDefs.h"
#include "UdpSocket.h"
#include "UdpEndpointAThread.h"

#define UDP_ENDPOINT_A_THREAD_BUFFER_LENGTH (1 * 1024 * 1024) // 1 MB

UdpEndpointAThread::UdpEndpointAThread(const U32 ipVer, const std::string& localIpAddrStr, const U16 localPortNumber)
: mIpVer(ipVer), mLocalIpAddrStr(localIpAddrStr), mLocalPortNumber(localPortNumber), mShutdown(false)
{
}

UdpEndpointAThread::~UdpEndpointAThread()
{
}

void UdpEndpointAThread::start()
{
	try
	{
		mpThread = new std::thread(&UdpEndpointAThread::run, this);
	}
	catch (const std::bad_alloc& e)
	{
		std::cout << "[Err] " << "Fail to create an instance of UdpEndpointAThread!" << std::endl;
		return;
	}
}

void UdpEndpointAThread::run()
{
//	InfoLog(<<"UdpEndpointAThread start.");
	std::cout << "[Info] " << "UdpEndpointAThread start." << std::endl;

	S32 ret;
	S_IpAddr sIpAddr;
	U16 portNumber;
	UdpSocket socket;
	U8 rxBuffer[UDP_ENDPOINT_A_THREAD_BUFFER_LENGTH];
	std::string rxMsg, txMsg;

	/* Open and configure a UDP Endpoint Socket with addr and port. */
	ret = socket.openAndConfig(mIpVer, O_NONBLOCK,
			UDP_ENDPOINT_A_THREAD_BUFFER_LENGTH, UDP_ENDPOINT_A_THREAD_BUFFER_LENGTH,
			mLocalIpAddrStr, mLocalPortNumber); // Non-blocking I/O
	if (STATUS_ERR == ret)
	{
//		ErrLog(<<"Fail to open and configure a UDP Endpoint Socket with addr and port!");
		std::cout << "[Err] " << "Fail to open and configure a UDP Endpoint Socket with addr and port!" << std::endl;
		return;
	}

	while (!waitForShutdown(10)) // Wait for 10 ms.
	{
		while (true)
		{
			ret = socket.recv(rxBuffer, UDP_ENDPOINT_A_THREAD_BUFFER_LENGTH, 0, sIpAddr, portNumber);
			if (ret <= 0)
			{
				/* No received message. */
				break;
			}
			else
			{
				/* A received message. */

				rxMsg.assign((char*)rxBuffer, ret);
				std::cout << "[Info] " << "UDP Endpoint Rx [" << std::dec << ret << " bytes]" << std::endl;
				std::cout << "[Info] " << "UDP Endpoint Rx: " << rxMsg << std::endl;

				/* Send a message. */
				txMsg.assign(UDP_ENDPOINT_A_TX_MSG);
				ret = socket.send(txMsg.c_str(), txMsg.length(), 0, sIpAddr, portNumber);
				if (ret != (S32)txMsg.length())
				{
	//				WarningLog(<<"Fail to send a message to a UDP Peer Endpoint!");
					std::cout << "[Warn] " << "Fail to send a message to a UDP Peer Endpoint!" << std::endl;
					continue;
				}
				std::cout << "[Info] " << "UDP Endpoint Tx [" << std::dec << ret << " bytes]" << std::endl;
			}
		}
	}

//	InfoLog(<<"UdpEndpointAThread exit.");
	std::cout << "[Info] " << "UdpEndpointAThread exit." << std::endl;
}

void UdpEndpointAThread::shutdown()
{
	std::unique_lock<std::mutex> lock(mMutex);

	if (!mShutdown)
	{
		mShutdown = true;
		mCondition.notify_one();
	}
}

bool UdpEndpointAThread::isShutdown() const
{
	std::unique_lock<std::mutex> lock(mMutex);

	return mShutdown;
}

bool UdpEndpointAThread::waitForShutdown(const S64 ms) const
{
	std::unique_lock<std::mutex> lock(mMutex);

	if (!mShutdown)
	{
		mCondition.wait_for(lock, std::chrono::milliseconds(ms));
	}

	return mShutdown;
}

/* End of File */

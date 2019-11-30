#include <stdio.h>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>
//#include "rutil/Logger.hxx"
#include "MacroDefs.h"
#include "UdpSocket.h"
#include "UdpEndpointBThread.h"

#define UDP_ENDPOINT_B_THREAD_BUFFER_LENGTH (1 * 1024 * 1024) // 1 MB

UdpEndpointBThread::UdpEndpointBThread(const U32 ipVer, const std::string& localIpAddrStr, const U16 localPortNumber,
		const std::string& remoteIpAddrStr, const U16 remotePortNumber)
: mIpVer(ipVer), mLocalIpAddrStr(localIpAddrStr), mLocalPortNumber(localPortNumber),
  mRemoteIpAddrStr(remoteIpAddrStr), mRemotePortNumber(remotePortNumber)
{
}

UdpEndpointBThread::~UdpEndpointBThread()
{
}

void UdpEndpointBThread::start()
{
	try
	{
		mpThread = new std::thread(&UdpEndpointBThread::run, this);
	}
	catch (const std::bad_alloc& e)
	{
		std::cout << "[Err] " << "Fail to create an instance of UdpEndpointBThread!" << std::endl;
		return;
	}
}

void UdpEndpointBThread::run()
{
//	InfoLog(<<"UdpEndpointBThread start.");
	std::cout << "[Info] " << "UdpEndpointBThread start." << std::endl;

	S32 ret;
	S_IpAddr sIpAddr;
	U16 portNumber;
	UdpSocket socket;
	U8 rxBuffer[UDP_ENDPOINT_B_THREAD_BUFFER_LENGTH];
	std::string rxMsg;
	char txMsg[1024];

	/* Open and configure a UDP Endpoint Socket with addr and port. */
	ret = socket.openAndConfig(mIpVer, 0,
			UDP_ENDPOINT_B_THREAD_BUFFER_LENGTH, UDP_ENDPOINT_B_THREAD_BUFFER_LENGTH,
			mLocalIpAddrStr, mLocalPortNumber); // blocking I/O
	if (STATUS_ERR == ret)
	{
//		ErrLog(<<"Fail to open and configure a UDP Endpoint Socket with addr and port!");
		std::cout << "[Err] " << "Fail to open and configure a UDP Endpoint Socket with addr and port!" << std::endl;
		return;
	}

	for (U32 i = 0; i < UDP_ENDPOINT_B_TX_MSG_NUM; i++)
	{
		/* Send a message. */
		sprintf(txMsg, UDP_ENDPOINT_B_TX_MSG, i, mLocalPortNumber);
		sIpAddr.ver = IP_ADDR_VER_4;
		if (1 != inet_pton(AF_INET, UDP_ENDPOINT_A_IP_ADDR, sIpAddr.addr))
		{
			std::cout << "[Warn] " << "Fail to convert a presentation format address to a network format address!" << std::endl;
			continue;
		}
		ret = socket.send(txMsg, strlen(txMsg), 0, sIpAddr, UDP_ENDPOINT_A_PORT_NUMBER);
		if (ret != (S32)strlen(txMsg))
		{
//			WarningLog(<<"Fail to send a message to the UDP Peer Endpoint!");
			std::cout << "[Warn] " << "Fail to send a message to the UDP Peer Endpoint!" << std::endl;
			continue;
		}
		std::cout << "[Info] " << "UDP Endpoint (port: " << mLocalPortNumber << ") Tx (" << i << ") [" << ret << " bytes]" << std::endl;

		ret = socket.recv(rxBuffer, UDP_ENDPOINT_B_THREAD_BUFFER_LENGTH, 0, sIpAddr, portNumber);
		if (ret <= 0)
		{
			/* No received message. */
			break;
		}
		else
		{
			/* A received message. */

			rxMsg.assign((char*)rxBuffer, ret);
			std::cout << "[Info] " << "UDP Endpoint (port: " << mLocalPortNumber << ") Rx (" << i << ") [" << ret << " bytes]" << std::endl;
			std::cout << "[Info] " << "UDP Endpoint (port: " << mLocalPortNumber << ") Rx (" << i << "): " << rxMsg << std::endl;
		}

		std::this_thread::sleep_for(std::chrono::seconds(UDP_ENDPOINT_B_TX_MSG_INTERVAL));
	}

//	InfoLog(<<"UdpEndpointBThread exit.");
	std::cout << "[Info] " << "UdpEndpointBThread exit." << std::endl;
}

/* End of File */

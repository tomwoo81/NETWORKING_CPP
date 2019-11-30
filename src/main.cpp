#include <unistd.h>
#include <string>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "MacroDefs.h"
#include "CommonTypes.h"
#include "IpAddr.h"
#include "TcpServerListenThread.h"
#include "TcpClientThread.h"
#include "UdpEndpointAThread.h"
#include "UdpEndpointBThread.h"

void tcpMain()
{
	std::cout << "[Info] " << "Main process - enter" << std::endl;

	pid_t pid;

	pid = fork();
	if (pid < 0)
	{
		/* Main process */
		std::cout << "[Err] " << "Fail to fork a child process!" << std::endl;
		std::cout << "[Info] " << "Main process - exit" << std::endl;
		exit(STATUS_ERR);
	}
	else if (0 == pid)
	{
		/* Child process for TCP server */
		std::cout << "[Info] " << "Child process for TCP server - enter" << std::endl;

		TcpServerListenThread* pTcpServerListenThread;

		try
		{
			pTcpServerListenThread = new TcpServerListenThread
					(IP_ADDR_VER_4, TCP_SERVER_IP_ADDR, TCP_SERVER_PORT_NUMBER);
		}
		catch (const std::bad_alloc& e)
		{
//			ErrLog(<<"Fail to create an instance of TcpServerListenThread!");
			std::cout << "[Err] " << "Fail to create an instance of TcpServerListenThread!" << std::endl;
			std::cout << "[Info] " << "Child process for TCP server - exit" << std::endl;
			exit(STATUS_ERR);
		}

		pTcpServerListenThread->start();

		std::this_thread::sleep_for(std::chrono::seconds(TCP_SERVER_RUNNING_DURATION));

		pTcpServerListenThread->shutdown();
		pTcpServerListenThread->join();
		delete pTcpServerListenThread;

		std::cout << "[Info] " << "Child process for TCP server - exit" << std::endl;
		exit(STATUS_OK);
	}
	else
	{
		/* Main process */
		for (U32 i = 0; i < NUM_TCP_CLIENTS; i++)
		{
			std::this_thread::sleep_for(std::chrono::seconds(TCP_CLIENT_TX_MSG_INTERVAL));

			pid = fork();
			if (pid < 0)
			{
				/* Main process */
				std::cout << "[Err] " << "Fail to fork a child process!" << std::endl;
				std::cout << "[Info] " << "Main process - exit" << std::endl;
				exit(STATUS_ERR);
			}
			else if (0 == pid)
			{
				/* Child process for TCP client */
				std::cout << "[Info] " << "Child process for TCP client " << i << " - enter" << std::endl;

				TcpClientThread* pTcpClientThread;

				try
				{
					pTcpClientThread = new TcpClientThread(IP_ADDR_VER_4, TCP_CLIENT_IP_ADDR, TCP_CLIENT_PORT_NUMBER_BASE + i,
							TCP_SERVER_IP_ADDR, TCP_SERVER_PORT_NUMBER);
				}
				catch (const std::bad_alloc& e)
				{
		//			ErrLog(<<"Fail to create an instance of TcpClientThread!");
					std::cout << "[Err] " << "Fail to create an instance of TcpClientThread!" << std::endl;
					std::cout << "[Info] " << "Child process for TCP client " << i << " - exit" << std::endl;
					exit(STATUS_ERR);
				}

				pTcpClientThread->start();

				pTcpClientThread->join();
				delete pTcpClientThread;

				std::cout << "[Info] " << "Child process for TCP client " << i << " - exit" << std::endl;
				exit(STATUS_OK);
			}
		}

		/* Wait for shutdown of all child processes. */
		while (STATUS_ERR != wait(NULL))
		{
			;
		}

		std::cout << "[Info] " << "Main process - exit" << std::endl;
		exit(STATUS_OK);
	}
}

void udpMain()
{
	std::cout << "[Info] " << "Main process - enter" << std::endl;

	pid_t pid;

	pid = fork();
	if (pid < 0)
	{
		/* Main process */
		std::cout << "[Err] " << "Fail to fork a child process!" << std::endl;
		std::cout << "[Info] " << "Main process - exit" << std::endl;
		exit(STATUS_ERR);
	}
	else if (0 == pid)
	{
		/* Child process for UDP endpoint A */
		std::cout << "[Info] " << "Child process for UDP endpoint A - enter" << std::endl;

		UdpEndpointAThread* pUdpEndpointAThread;

		try
		{
			pUdpEndpointAThread = new UdpEndpointAThread
					(IP_ADDR_VER_4, UDP_ENDPOINT_A_IP_ADDR, UDP_ENDPOINT_A_PORT_NUMBER);
		}
		catch (const std::bad_alloc& e)
		{
//			ErrLog(<<"Fail to create an instance of UdpEndpointAThread!");
			std::cout << "[Err] " << "Fail to create an instance of UdpEndpointAThread!" << std::endl;
			std::cout << "[Info] " << "Child process for UDP endpoint A - exit" << std::endl;
			exit(STATUS_ERR);
		}

		pUdpEndpointAThread->start();

		std::this_thread::sleep_for(std::chrono::seconds(UDP_ENDPOINT_A_RUNNING_DURATION));

		pUdpEndpointAThread->shutdown();
		pUdpEndpointAThread->join();
		delete pUdpEndpointAThread;

		std::cout << "[Info] " << "Child process for UDP endpoint A - exit" << std::endl;
		exit(STATUS_OK);
	}
	else
	{
		/* Main process */
		for (U32 i = 0; i < NUM_UDP_ENDPOINTS_B; i++)
		{
			std::this_thread::sleep_for(std::chrono::seconds(UDP_ENDPOINT_B_TX_MSG_INTERVAL));

			pid = fork();
			if (pid < 0)
			{
				/* Main process */
				std::cout << "[Err] " << "Fail to fork a child process!" << std::endl;
				std::cout << "[Info] " << "Main process - exit" << std::endl;
				exit(STATUS_ERR);
			}
			else if (0 == pid)
			{
				/* Child process for UDP endpoint B */
				std::cout << "[Info] " << "Child process for UDP endpoint B " << i << " - enter" << std::endl;

				UdpEndpointBThread* pUdpEndpointBThread;

				try
				{
					pUdpEndpointBThread = new UdpEndpointBThread(IP_ADDR_VER_4, UDP_ENDPOINT_B_IP_ADDR, UDP_ENDPOINT_B_PORT_NUMBER_BASE + i,
							UDP_ENDPOINT_A_IP_ADDR, UDP_ENDPOINT_A_PORT_NUMBER);
				}
				catch (const std::bad_alloc& e)
				{
		//			ErrLog(<<"Fail to create an instance of UdpEndpointBThread!");
					std::cout << "[Err] " << "Fail to create an instance of UdpEndpointBThread!" << std::endl;
					std::cout << "[Info] " << "Child process for UDP endpoint B " << i << " - exit" << std::endl;
					exit(STATUS_ERR);
				}

				pUdpEndpointBThread->start();

				pUdpEndpointBThread->join();
				delete pUdpEndpointBThread;

				std::cout << "[Info] " << "Child process for UDP endpoint B " << i << " - exit" << std::endl;
				exit(STATUS_OK);
			}
		}

		/* Wait for shutdown of all child processes. */
		while (STATUS_ERR != wait(NULL))
		{
			;
		}

		std::cout << "[Info] " << "Main process - exit" << std::endl;
		exit(STATUS_OK);
	}
}

int main(int argc, char *argv[])
{
	std::string socketType;

	if (argc >= 2)
	{
		socketType = argv[1];
	}
	else
	{
		socketType = "tcp";
	}

	if (0 == strcasecmp("tcp", socketType.c_str()))
	{
		tcpMain();
	}
	else if (0 == strcasecmp("udp", socketType.c_str()))
	{
		udpMain();
	}
	else
	{
		std::cout << "[Err] " << "The type (" << socketType << ") is invalid!" << std::endl;
        return STATUS_ERR;
	}

	return STATUS_OK;
}

/* End of File */

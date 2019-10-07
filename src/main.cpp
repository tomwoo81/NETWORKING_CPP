#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include "MacroDefs.h"
#include "CommonTypes.h"
#include "TcpServerListenThread.h"
#include "TcpClientThread.h"

int main(int argc, char *argv[])
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
	else if(0 == pid)
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

		pthread_kill(pTcpServerListenThread->native_handle(), SIGTERM);
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
		while(STATUS_ERR != wait(NULL))
		{
			;
		}

		std::cout << "[Info] " << "Main process - exit" << std::endl;
		exit(STATUS_OK);
	}

	return STATUS_OK;
}

/* End of File */

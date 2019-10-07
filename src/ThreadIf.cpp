#include <iostream>
#include "ThreadIf.h"

ThreadIf::ThreadIf()
: mpThread(NULL)
{
}

ThreadIf::~ThreadIf()
{
	if (NULL != mpThread)
	{
		delete mpThread;
		mpThread = NULL;
	}
}

std::thread::native_handle_type ThreadIf::native_handle()
{
	if (NULL == mpThread)
	{
		std::cout << "[Err] " << "Pointer to the thread is NULL!" << std::endl;
		return NULL;
	}

	return mpThread->native_handle();
}

void ThreadIf::join()
{
	if (NULL == mpThread)
	{
		std::cout << "[Err] " << "Pointer to the thread is NULL!" << std::endl;
		return;
	}

	mpThread->join();
}

/* End of File */

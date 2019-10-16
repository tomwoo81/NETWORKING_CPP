#include <iostream>
#include "ThreadIf.h"

ThreadIf::ThreadIf()
: mpThread(nullptr)
{
}

ThreadIf::~ThreadIf()
{
	if (nullptr != mpThread)
	{
		delete mpThread;
		mpThread = nullptr;
	}
}

void ThreadIf::join()
{
	if (nullptr == mpThread)
	{
		std::cout << "[Err] " << "Pointer to the thread is NULL!" << std::endl;
		return;
	}

	mpThread->join();
}

/* End of File */

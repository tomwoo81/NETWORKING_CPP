#pragma once

#include <thread>

class ThreadIf
{
public:
	ThreadIf();
	virtual ~ThreadIf();

	virtual void start() = 0;
	virtual void run() = 0;
	void join();

protected:
	std::thread* mpThread;
};

/* End of File */

//#include "rutil/Logger.hxx"
#include <iostream>
#include <pthread.h>
#include "ThreadPool.h"

PoolTaskIf::PoolTaskIf()
: mpThreadPool(NULL)
{
}

PoolTaskIf::PoolTaskIf(const std::string& taskNameStr)
: mTaskNameStr(taskNameStr), mpThreadPool(NULL)
{
}

PoolTaskIf::~PoolTaskIf()
{
}

void PoolTaskIf::setThreadPoolPtr(ThreadPool* const pThreadPool)
{
	mpThreadPool = pThreadPool;
}

void PoolTaskIf::mSleep(const U16 ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}


void PoolThreadRun(PoolThread* const pThread)
{
	std::cout << "[Info] " << "PoolThread - enter" << std::endl;

	if (NULL == pThread)
	{
		std::cout << "[Err] " << "Pointer to the instance of PoolThread is NULL!" << std::endl;
		std::cout << "[Info] " << "PoolThread - exit" << std::endl;
		return;
	}

	pThread->run();

	std::cout << "[Info] " << "PoolThread - exit" << std::endl;
}

PoolThread::PoolThread(ThreadPool* const pThreadPool)
: mpThreadPool(pThreadPool), mpTask(NULL)
{
}

PoolThread::~PoolThread()
{
	if (NULL != mpTask)
	{
		delete mpTask;
		mpTask = NULL;
	}
}

void PoolThread::start()
{
	try
	{
		mpThread = new std::thread(PoolThreadRun, this);
	}
	catch (const std::bad_alloc& e)
	{
		std::cout << "[Err] " << "Fail to create an instance of PoolThread!" << std::endl;
		return;
	}
}

void PoolThread::run()
{
	std::thread::id tid = std::this_thread::get_id();

	while (1)
	{
		if (mpThreadPool->getTask(mpTask))
		{
//			InfoLog(<<"Thread "<<tid<<" will be busy.");
			std::cout << "[Info] " << "Thread " << tid << " will be busy." << std::endl;
			mpTask->run();
			delete mpTask; //Delete a task.
			mpTask = NULL;
//			InfoLog(<<"Thread "<<tid<<" will be idle.");
			std::cout << "[Info] " << "Thread " << tid << " will be idle." << std::endl;
		}
		else
		{
//			InfoLog(<<"Thread "<<tid<<" will exit.");
			std::cout << "[Info] " << "Thread " << tid << " will exit." << std::endl;
			break;
		}
	}
}


ThreadPool::ThreadPool(const U32 numThreads)
{
	if (STATUS_ERR == createAll(numThreads))
	{
		return;
	}

//	InfoLog(<<numThreads<<" threads are created in this Thread Pool.");
	std::cout << "[Info] " << numThreads << " threads are created in this Thread Pool." << std::endl;
}

ThreadPool::~ThreadPool()
{
	shutdownAll();
}

void ThreadPool::addTask(PoolTaskIf* const pTask)
{
	std::unique_lock<std::mutex> lock(mMutex);

	pTask->setThreadPoolPtr(this);
	mTaskVec.push_back(pTask);
	mCondition.notify_one();
}

bool ThreadPool::getTask(PoolTaskIf*& pTask)
{
	std::unique_lock<std::mutex> lock(mMutex);

	while ((0 == mTaskVec.size()))
	{
		mCondition.wait(lock);
	}

	std::vector<PoolTaskIf*>::iterator iter = mTaskVec.begin();
	if (iter != mTaskVec.end())
	{
		pTask = *iter;
		mTaskVec.erase(iter);
	}

	return true;
}

void ThreadPool::shutdownAll()
{
	std::unique_lock<std::mutex> lock(mMutex);

	for (auto p: mThreadVec)
	{
		pthread_kill(p->native_handle(), SIGTERM);
		p->join();
		delete p;
	}
	mThreadVec.clear();

	for (auto p: mTaskVec)
	{
		delete p;
	}
	mTaskVec.clear();
}

U32 ThreadPool::getPendingTaskNum() const
{
	std::unique_lock<std::mutex> lock(mMutex);

	return mTaskVec.size();
}

STATUS ThreadPool::createAll(const U32 numThreads)
{
	PoolThread* pThread;

	for (U32 i = 0; i < numThreads; i++)
	{
		try
		{
			pThread = new PoolThread(this);
		}
		catch (const std::bad_alloc& e)
		{
//			ErrLog(<<"Fail to create a thread!");
			std::cout << "[Err] " << "Fail to create a thread!" << std::endl;
			return STATUS_ERR;
		}

		pThread->start();
		mThreadVec.push_back(pThread);
	}

	return STATUS_OK;
}

/* End of File */
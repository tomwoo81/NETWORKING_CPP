#include <iostream>
#include <iomanip>
#include <functional>
//#include "rutil/Logger.hxx"
#include "ThreadPool.h"

PoolTaskIf::PoolTaskIf()
: mpThreadPool(nullptr)
{
}

PoolTaskIf::PoolTaskIf(const std::string& taskNameStr)
: mTaskNameStr(taskNameStr), mpThreadPool(nullptr)
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


PoolThread::PoolThread(ThreadPool* const pThreadPool)
: mpThreadPool(pThreadPool), mpTask(nullptr)
{
}

PoolThread::~PoolThread()
{
	if (nullptr != mpTask)
	{
		delete mpTask;
		mpTask = nullptr;
	}
}

void PoolThread::start()
{
	try
	{
		mpThread = new std::thread(std::bind(&PoolThread::run, this));
	}
	catch (const std::bad_alloc& e)
	{
		std::cout << "[Err] " << "Fail to create an instance of PoolThread!" << std::endl;
		return;
	}
}

void PoolThread::run()
{
	std::cout << "[Info] " << "PoolThread - enter" << std::endl;

	std::thread::id tid = std::this_thread::get_id();

	while (true)
	{
		if (mpThreadPool->getTask(mpTask))
		{
			if (nullptr != mpTask)
			{
//				InfoLog(<<"Thread "<<tid<<" will be busy.");
				std::cout << "[Info] " << "Thread " << std::setiosflags(std::ios::showbase | std::ios::uppercase) << std::hex << tid << " will be busy." << std::endl;
				mpTask->run();
				delete mpTask; // Delete a task.
				mpTask = nullptr;
//				InfoLog(<<"Thread "<<tid<<" will be idle.");
				std::cout << "[Info] " << "Thread " << std::setiosflags(std::ios::showbase | std::ios::uppercase) << std::hex << tid << " will be idle." << std::endl;
			}
		}
		else
		{
//			InfoLog(<<"Thread "<<tid<<" will exit.");
			std::cout << "[Info] " << "Thread " << std::setiosflags(std::ios::showbase | std::ios::uppercase) << std::hex << tid << " will exit." << std::endl;
			break;
		}
	}

	std::cout << "[Info] " << "PoolThread - exit" << std::endl;
}


ThreadPool::ThreadPool(const U32 numThreads)
: mShutdown(false)
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
	joinAll();
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

	while ((0 == mTaskVec.size()) && (!mShutdown))
	{
		mCondition.wait(lock);
	}

	if (mShutdown)
	{
		return false;
	}

	std::vector<PoolTaskIf*>::iterator iter = mTaskVec.begin();
	if (iter != mTaskVec.end())
	{
		pTask = *iter;
		mTaskVec.erase(iter);
	}
	else
	{
		pTask = nullptr;
	}

	return true;
}

bool ThreadPool::isShutdown() const
{
	std::unique_lock<std::mutex> lock(mMutex);

	return mShutdown;
}

void ThreadPool::shutdownAll()
{
	std::unique_lock<std::mutex> lock(mMutex);

	if (!mShutdown)
	{
//		InfoLog(<<"To shutdown all threads in this Thread Pool.");
		std::cout << "[Info] " << "To shutdown all threads in this Thread Pool." << std::endl;

		mShutdown = true;
		mCondition.notify_all();
	}
}

void ThreadPool::joinAll()
{
	for (auto p: mThreadVec)
	{
		p->join();
		delete p;
	}

	mThreadVec.clear();
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

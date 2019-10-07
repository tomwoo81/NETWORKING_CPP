#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include "CommonTypes.h"
#include "ThreadIf.h"

class ThreadPool;

class PoolTaskIf
{
public:
	PoolTaskIf();
	PoolTaskIf(const std::string& taskNameStr);
	virtual ~PoolTaskIf();

	void setThreadPoolPtr(ThreadPool* const pThreadPool);
	virtual void run() = 0;

protected:
	void mSleep(const U16 ms);

	std::string mTaskNameStr;
	ThreadPool* mpThreadPool;
};

class PoolThread: public ThreadIf
{
public:
	PoolThread(ThreadPool* const pThreadPool);
	virtual ~PoolThread();

	virtual void start();
	virtual void run();

private:
	ThreadPool* mpThreadPool;
	PoolTaskIf* mpTask;
};

class ThreadPool
{
public:
	ThreadPool(const U32 numThreads);
	virtual ~ThreadPool();

	void addTask(PoolTaskIf* const pTask);
	bool getTask(PoolTaskIf*& pTask);
	void shutdownAll();
	U32 getPendingTaskNum() const;

private:
	STATUS createAll(const U32 numThreads);

	std::vector<PoolThread*> mThreadVec;

	std::vector<PoolTaskIf*> mTaskVec;
	mutable std::mutex mMutex;
	mutable std::condition_variable mCondition;
};

/* End of File */

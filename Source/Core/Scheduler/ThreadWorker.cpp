// <copyright file="ThreadWorker.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "ThreadWorker.h"

namespace PAL
{
	ThreadWorker::ThreadWorker(Scheduler& scheduler) :
		Worker(scheduler),
		_running(false)
	{
	}

	void ThreadWorker::Initialize()
	{
		_running = true;
		_thread = std::make_unique<std::thread>(std::bind(&Worker::MainLoop, this));
	}

	void ThreadWorker::Shutdown()
	{
		_running = false;
	}

	void ThreadWorker::WaitJoin()
	{
		_thread->join();
		_thread = nullptr;
	}

	bool ThreadWorker::IsRunning()
	{
		return _running;
	}

} // PAL
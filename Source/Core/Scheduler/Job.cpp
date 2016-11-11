// <copyright file="Job.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "Job.h"

#include "Fence.h"
#include "Scheduler.h"
#include "Worker.h"
#include "EventType.h"

namespace PAL
{
	uint32_t Job::_uniqueId = 1;

	Job::Job(std::function<void(void)> function, bool runOnMainThread, Fence* fence) :
		_id(_uniqueId++),
		_function(function),
		_runOnMainThread(runOnMainThread),
		_fence(fence)
	{
	}

	Job::~Job()
	{
	}

	uint32_t Job::GetId() const
	{
		return _id;
	}

	bool Job::RunOnMainThread() const
	{
		return _runOnMainThread;
	}

	void Job::Execute()
	{
		Worker::GetActive().GetLogger().LogEvent(EventType::JobBegin, GetId());
		_function();
		Worker::GetActive().GetLogger().LogEvent(EventType::JobEnd, GetId());

		// Signal that this job has finished
		if (_fence != nullptr && _fence->Signal())
		{
			// We have finished running all jobs for this fence
			Worker::GetActive().GetScheduler().UpdateProcess();
		}
	}

} // PAL
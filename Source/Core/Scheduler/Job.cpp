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
	Job::Job(std::function<void(void)>&& function, bool runOnMainThread, Fence* fence) :
		m_function(function),
		m_runOnMainThread(runOnMainThread),
		m_fence(fence)
	{
		static uint32_t s_uniqueId = 0;
		m_id = s_uniqueId++;
	}

	Job::~Job()
	{
	}

	uint32_t Job::GetId() const
	{
		return m_id;
	}

	bool Job::RunOnMainThread() const
	{
		return m_runOnMainThread;
	}

	void Job::Execute()
	{
		Worker::GetActive().GetLogger().LogEvent(EventType::JobBegin, GetId());
		m_function();
		Worker::GetActive().GetLogger().LogEvent(EventType::JobEnd, GetId());

		// Signal that this job has finished
		if (m_fence != nullptr && m_fence->Signal())
		{
			// We have finished running all jobs for this fence
			Worker::GetActive().GetScheduler().UpdateProcess();
		}
	}

} // PAL
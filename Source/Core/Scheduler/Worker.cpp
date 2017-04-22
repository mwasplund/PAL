// <copyright file="Worker.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "Worker.h"
#include "EventType.h"
#include "Job.h"

namespace PAL
{
	/*static*/ thread_local Worker* Worker::ActiveWorker = nullptr;

	/*static*/ Worker& Worker::GetActive()
	{
		if (ActiveWorker == nullptr)
		{
			throw std::runtime_error("No worker was registered for this thread.");
		}

		return *ActiveWorker;
	}

	Worker::Worker(Scheduler& scheduler) :
		m_mutex(),
		m_scheduler(scheduler),
		m_jobs(),
		m_completedJobCount(0),
		m_logger()
	{
		static uint32_t s_uniqueId = 1;
		m_id = s_uniqueId++;
	}

	void Worker::MainLoop()
	{
		m_logger.LogEvent(EventType::WorkerBegin, m_id);

		// Register this worker for this thread
		if (ActiveWorker != nullptr)
		{
			throw std::runtime_error("Tried to register a second worker for this thread.");
		}

		ActiveWorker = this;

		while (IsRunning())
		{
			std::shared_ptr<Job> job = FindWork();
			if (job != nullptr)
			{
				m_logger.LogEvent(EventType::WorkerExecuteJobBegin, m_id);
				job->Execute();
				m_completedJobCount++;
				m_logger.LogEvent(EventType::WorkerExecuteJobEnd, m_id);
			}
		}

		// Remove this worker from the thread active worker
		ActiveWorker = nullptr;

		m_logger.LogEvent(EventType::WorkerEnd, m_id);
	}

	void Worker::RegisterJobs(std::vector<std::shared_ptr<Job>>&& jobs)
	{
		// lock to ensure we are the only ones touching the jobs
		std::lock_guard<std::mutex> lock(m_mutex);

		for (std::shared_ptr<Job>& job : jobs)
		{
			m_jobs.push(std::move(job));
		}
	}

	uint64_t Worker::GetCompletedJobCount() const
	{
		return m_completedJobCount;
	}

	EventLogger& Worker::GetLogger()
	{
		return m_logger;
	}

	Scheduler& Worker::GetScheduler()
	{
		return m_scheduler;
	}

	std::shared_ptr<Job> Worker::FindWork()
	{
		std::shared_ptr<Job> result = nullptr;

		// Check the local 
		{
			// lock to ensure we are the only ones touching the jobs
			std::lock_guard<std::mutex> lock(m_mutex);

			if (!m_jobs.empty())
			{
				result = std::move(m_jobs.front());
				m_jobs.pop();
			}
			else
			{
				// play nice
				// this_thread::sleep_for(10ms);
			}
		}

		return result;
	}

} // PAL
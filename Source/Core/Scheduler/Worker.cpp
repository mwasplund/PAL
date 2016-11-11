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

	Worker::Worker(Application& app) :
		_mutex(),
		_app(app),
		_jobs(),
		_completedJobCount(0),
		_logger()
	{
		static uint32_t uniqueId = 1;
		_id = uniqueId++;
	}

	void Worker::MainLoop()
	{
		_logger.LogEvent(EventType::WorkerBegin, _id);

		// Register this worker for this thread
		if (ActiveWorker != nullptr)
		{
			throw std::runtime_error("Tried to register a second worker for this thread.");
		}

		ActiveWorker = this;

		while (IsRunning())
		{
			std::shared_ptr<Job> job = nullptr;

			{
				// lock to ensure we are the only ones touching the jobs
				std::lock_guard<std::mutex> lock(_mutex);

				if (!_jobs.empty())
				{
					job = move(_jobs.front());
					_jobs.pop();
				}
				else
				{
					// play nice
					// this_thread::sleep_for(10ms);
				}
			}

			if (job != nullptr)
			{
				_logger.LogEvent(EventType::WorkerExecuteJobBegin, _id);
				job->Execute();
				_completedJobCount++;
				_logger.LogEvent(EventType::WorkerExecuteJobEnd, _id);
			}
		}

		// Remove this worker from the thread active worker
		ActiveWorker = nullptr;

		_logger.LogEvent(EventType::WorkerEnd, _id);
	}

	void Worker::RegisterJobs(std::vector<std::shared_ptr<Job>>&& jobs)
	{
		// lock to ensure we are the only ones touching the jobs
		std::lock_guard<std::mutex> lock(_mutex);

		for (std::shared_ptr<Job>& job : jobs)
		{
			_jobs.push(std::move(job));
		}
	}

	uint64_t Worker::GetCompletedJobCount() const
	{
		return _completedJobCount;
	}

	EventLogger& Worker::GetLogger()
	{
		return _logger;
	}

	Application& Worker::GetApp()
	{
		return _app;
	}

} // PAL
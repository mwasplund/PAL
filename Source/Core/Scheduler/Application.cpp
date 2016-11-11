// <copyright file="Application.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "Application.h"
#include "EventLogParser.h"
#include "EventLogGraph.h"
#include "Job.h"

namespace PAL
{
	Application::Application(std::unique_ptr<Process>&& mainProcess, size_t workerCount) :
		_mutex(),
		_mainProcess(move(mainProcess)),
		_mainWorker(std::make_unique<MainWorker>(*this)),
		_workerCount(workerCount),
		_backgroundWorkers(_workerCount - 1),
		_dispatchedJobsCount(0)
	{
		for (size_t i = 0; i < _backgroundWorkers.size(); i++)
		{
			_backgroundWorkers[i] = std::make_unique<ThreadWorker>(*this);
		}
	}

	void Application::Run()
	{
		// Dispatch the initial jobs
		_mainProcess->Initialize();
		UpdateProcess();

		// Initialize the background workers
		for (size_t i = 0; i < _backgroundWorkers.size(); i++)
		{
			_backgroundWorkers[i]->Initialize();
		}

		// Run the main worker
		_mainWorker->MainLoop();

		// Shutdown the background workers
		for (size_t i = 0; i < _backgroundWorkers.size(); i++)
		{
			_backgroundWorkers[i]->Shutdown();
		}

		// Wait until all workers are done running
		for (size_t i = 0; i < _backgroundWorkers.size(); i++)
		{
			_backgroundWorkers[i]->WaitJoin();
		}
	}

	void Application::UpdateProcess()
	{
		// Initialize the process and find the initial jobs
		std::vector<std::shared_ptr<Job>> pendingJobs = _mainProcess->GetAvailableJobs(*this);

		// Dispatch the initial jobs
		DispatchJobs(std::move(pendingJobs));
	}

	void Application::DispatchJob(std::shared_ptr<Job> job)
	{
		std::vector<std::shared_ptr<Job>> jobs = { job };
		DispatchJobs(std::move(jobs));
	}

	void Application::DispatchJobs(std::vector<std::shared_ptr<Job>>&& jobs)
	{
		// lock to ensure we are the only ones touching the jobs
		std::lock_guard<std::mutex> lock(_mutex);

		// Update the count of required jobs to complete
		_dispatchedJobsCount += jobs.size();

		// Subdivide all jobs 
		// Note: index 0 is the main worker
		std::vector<std::vector<std::shared_ptr<Job>>> subJobs;
		subJobs.resize(_workerCount);

		// Find the jobs that must run on the main thread
		for (size_t i = 0; i < jobs.size(); i++)
		{
			if (jobs[i]->RunOnMainThread())
			{
				subJobs[0].push_back(move(jobs[i]));
				jobs[i] = nullptr;
			}
		}

		// Dispatch the remaining jobs round robin style
		size_t workerIndex = 0;
		
		// Skip the main worker to even out jobs
		size_t skipMainWorkerCount = subJobs[0].size();

		for (size_t i = 0; i < jobs.size(); i++)
		{
			if (jobs[i] != nullptr)
			{
				if (workerIndex == 0 && skipMainWorkerCount > 0)
				{
					// Move to next worker
					workerIndex = (workerIndex + 1) % _workerCount;
					skipMainWorkerCount--;
				}

				// Create a sub list of jobs for this worker
				subJobs[workerIndex].push_back(move(jobs[i]));
				jobs[i] = nullptr;

				// Move to next worker
				workerIndex = (workerIndex + 1) % _workerCount;
			}
		}

		// Dispatch sub jobs to each worker
		for (size_t i = 0; i < _workerCount; i++)
		{
			if (subJobs[i].size() > 0)
			{
				// The first set is for the main worker
				if (i == 0)
				{
					_mainWorker->RegisterJobs(move(subJobs[i]));
				}
				else
				{
					_backgroundWorkers[i - 1]->RegisterJobs(move(subJobs[i]));
				}
			}
		}

		// Clear the collection of jobs
		jobs.clear();
	}

	bool Application::IsProcessCompleted()
	{
		// lock to ensure we are the only ones touching the tasks
		std::lock_guard<std::mutex> lock(_mutex);

		// Check if we have completed all dispatched jobs
		uint64_t completedSteps = 0;

		completedSteps += _mainWorker->GetCompletedJobCount();

		for (size_t i = 0; i < _backgroundWorkers.size(); i++)
		{
			completedSteps += _backgroundWorkers[i]->GetCompletedJobCount();
		}

		return _dispatchedJobsCount == completedSteps;
	}

	void Application::PrintLogs(std::chrono::high_resolution_clock::time_point referenceTime)
	{
		EventLogParser parser = EventLogParser();
		parser.Parse(referenceTime, _mainWorker->GetLogger().GetLog());

		for (size_t i = 0; i < _backgroundWorkers.size(); i++)
		{
			parser.Parse(referenceTime, _backgroundWorkers[i]->GetLogger().GetLog());
		}
	}

	void Application::GenerateGraph(const std::wstring& filename)
	{
		// Build up all logs
		std::vector<const EventLog*> logs;
		logs.push_back(&_mainWorker->GetLogger().GetLog());

		for (size_t i = 0; i < _backgroundWorkers.size(); i++)
		{
			logs.push_back(&_backgroundWorkers[i]->GetLogger().GetLog());
		}

		EventLogGraph graph = EventLogGraph();
		graph.Generate(filename, logs, _mainProcess->AssignJobColors());
	}

} // PAL
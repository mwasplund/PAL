// <copyright file="Scheduler.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "Scheduler.h"
#include "EventLogParser.h"
#include "EventLogGraph.h"
#include "Job.h"
#include "MainWorker.h"
#include "ThreadWorker.h"

namespace PAL 
{
	Scheduler::Scheduler(std::unique_ptr<Process>&& mainProcess, size_t workerCount) :
		m_mutex(),
		m_mainProcess(std::move(mainProcess)),
		m_mainWorker(std::make_unique<MainWorker>(*this)),
		m_workerCount(workerCount),
		m_backgroundWorkers(m_workerCount - 1),
		m_dispatchedJobsCount(0)
	{
		for (size_t i = 0; i < m_backgroundWorkers.size(); i++)
		{
			m_backgroundWorkers[i] = std::make_unique<ThreadWorker>(*this);
		}
	}

	void Scheduler::Run()
	{
		// Dispatch the initial jobs
		m_mainProcess->Initialize();
		UpdateProcess();

		// Initialize the background workers
		for (size_t i = 0; i < m_backgroundWorkers.size(); i++)
		{
			m_backgroundWorkers[i]->Initialize();
		}

		// Run the main worker
		m_mainWorker->MainLoop();

		// Shutdown the background workers
		for (size_t i = 0; i < m_backgroundWorkers.size(); i++)
		{
			m_backgroundWorkers[i]->Shutdown();
		}

		// Wait until all workers are done running
		for (size_t i = 0; i < m_backgroundWorkers.size(); i++)
		{
			m_backgroundWorkers[i]->WaitJoin();
		}
	}

	void Scheduler::UpdateProcess()
	{
		// Initialize the process and find the initial jobs
		std::vector<std::shared_ptr<Job>> pendingJobs = m_mainProcess->GetAvailableJobs(*this);

		// Dispatch the initial jobs
		DispatchJobs(std::move(pendingJobs));
	}

	void Scheduler::DispatchJob(std::shared_ptr<Job>&& job)
	{
		std::vector<std::shared_ptr<Job>> jobs = { std::move(job) };
		DispatchJobs(std::move(jobs));
	}

	void Scheduler::DispatchJobs(std::vector<std::shared_ptr<Job>>&& jobs)
	{
		// lock to ensure we are the only ones touching the jobs
		std::lock_guard<std::mutex> lock(m_mutex);

		// Update the count of required jobs to complete
		m_dispatchedJobsCount += jobs.size();

		// Subdivide all jobs 
		// Note: index 0 is the main worker
		std::vector<std::vector<std::shared_ptr<Job>>> subJobs;
		subJobs.resize(m_workerCount);

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
					workerIndex = (workerIndex + 1) % m_workerCount;
					skipMainWorkerCount--;
				}

				// Create a sub list of jobs for this worker
				subJobs[workerIndex].push_back(std::move(jobs[i]));
				jobs[i] = nullptr;

				// Move to next worker
				workerIndex = (workerIndex + 1) % m_workerCount;
			}
		}

		// Dispatch sub jobs to each worker
		for (size_t i = 0; i < m_workerCount; i++)
		{
			if (subJobs[i].size() > 0)
			{
				// The first set is for the main worker
				if (i == 0)
				{
					m_mainWorker->RegisterJobs(std::move(subJobs[i]));
				}
				else
				{
					m_backgroundWorkers[i - 1]->RegisterJobs(std::move(subJobs[i]));
				}
			}
		}

		// Clear the collection of jobs
		jobs.clear();
	}

	bool Scheduler::IsProcessCompleted()
	{
		// lock to ensure we are the only ones touching the tasks
		std::lock_guard<std::mutex> lock(m_mutex);

		// Check if we have completed all dispatched jobs
		uint64_t completedSteps = 0;

		completedSteps += m_mainWorker->GetCompletedJobCount();

		for (size_t i = 0; i < m_backgroundWorkers.size(); i++)
		{
			completedSteps += m_backgroundWorkers[i]->GetCompletedJobCount();
		}

		return m_dispatchedJobsCount == completedSteps;
	}

	void Scheduler::PrintLogs(std::chrono::high_resolution_clock::time_point referenceTime)
	{
		EventLogParser parser = EventLogParser();
		parser.Parse(referenceTime, m_mainWorker->GetLogger().GetLog());

		for (size_t i = 0; i < m_backgroundWorkers.size(); i++)
		{
			parser.Parse(referenceTime, m_backgroundWorkers[i]->GetLogger().GetLog());
		}
	}

	void Scheduler::GenerateGraph(const std::wstring& filename)
	{
		// Build up all logs
		std::vector<const EventLog*> logs;
		logs.push_back(&m_mainWorker->GetLogger().GetLog());

		for (size_t i = 0; i < m_backgroundWorkers.size(); i++)
		{
			logs.push_back(&m_backgroundWorkers[i]->GetLogger().GetLog());
		}

		EventLogGraph graph = EventLogGraph();
		graph.Generate(filename, logs, m_mainProcess->AssignJobColors());
	}

} // PAL
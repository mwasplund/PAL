// <copyright file="Scheduler.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once
#include "Process.h"
#include "MainWorker.h"
#include "ThreadWorker.h"

namespace PAL 
{
	/// <summary>
	/// The task scheduler
	/// </summary>
	class Scheduler
	{
	public:
		/// <summary>
		/// Initialize a new instance of the Scheduler class
		/// </summary>
		Scheduler(std::unique_ptr<Process>&& mainProcess, size_t workerCount);

		/// <summary>
		/// Run
		/// </summary>
		void Run();

		/// <summary>
		/// Update the process
		/// </summary>
		void UpdateProcess();

		/// <summary>
		/// Dispatch job
		/// </summary>
		void DispatchJob(std::shared_ptr<Job>&& job);

		/// <summary>
		/// Dispatch jobs
		/// </summary>
		void DispatchJobs(std::vector<std::shared_ptr<Job>>&& jobs);

		/// <summary>
		/// Get a value indicating whether the process is completed or not
		/// </summary>
		bool IsProcessCompleted();

		/// <summary>
		/// Print Logs
		/// </summary>
		void PrintLogs(std::chrono::high_resolution_clock::time_point referenceTime);

		/// <summary>
		/// Generate Graph
		/// </summary>
		void GenerateGraph(const std::wstring& filename);

	private:
		/// <summary>
		/// The mutex
		/// </summary>
		std::mutex m_mutex;

		/// <summary>
		/// The main process
		/// </summary>
		std::unique_ptr<Process> m_mainProcess;

		/// <summary>
		/// The main worker
		/// </summary>
		std::unique_ptr<MainWorker> m_mainWorker;

		/// <summary>
		/// The number of workers
		/// </summary>
		size_t m_workerCount;

		/// <summary>
		/// The background workers
		/// </summary>
		std::vector<std::unique_ptr<ThreadWorker>> m_backgroundWorkers;

		/// <summary>
		/// The count of dispatched jobs
		/// </summary>
		uint64_t m_dispatchedJobsCount;
	};
} // PAL
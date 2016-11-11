// <copyright file="Worker.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once
#include "EventLogger.h"

namespace PAL
{
	/// <summary>
	/// The worker
	/// </summary>
	class Worker
	{
	private:
		/// <summary>
		/// The thread local active worker
		/// </summary>
		static thread_local Worker* ActiveWorker;

	public:
		/// <summary>
		/// Get the active worker for this thread
		/// </summary>
		static Worker& GetActive();

		/// <summary>
		/// Initialize a new instance of the Worker class
		/// </summary>
		Worker(Application& app);

		/// <summary>
		/// Run Main Main Loop
		/// </summary>
		void MainLoop();

		/// <summary>
		/// Register jobs
		/// </summary>
		void RegisterJobs(std::vector<std::shared_ptr<Job>>&& jobs);

		/// <summary>
		/// Get Completed job count
		/// </summary>
		uint64_t GetCompletedJobCount() const;

		/// <summary>
		/// Get the event logger
		/// </summary>
		EventLogger& GetLogger();

		/// <summary>
		/// Get the application
		/// </summary>
		Application& GetApp();

	protected:
		/// <summary>
		/// Is Running
		/// </summary>
		virtual bool IsRunning() = 0;

		/// <summary>
		/// The application
		/// </summary>
		Application& _app;

		/// <summary>
		/// The mutex
		/// </summary>
		std::mutex _mutex;

		/// <summary>
		/// The count of completed jobs
		/// </summary>
		uint64_t _completedJobCount;

		/// <summary>
		/// The pending jobs on this worker
		/// </summary>
		std::queue<std::shared_ptr<Job>> _jobs;

		/// <summary>
		/// The event logger
		/// </summary>
		EventLogger _logger;

		/// <summary>
		/// The worker id
		/// </summary>
		uint32_t _id;
	};

} // PAL
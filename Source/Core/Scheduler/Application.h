// <copyright file="Application.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once
#include "Process.h"
#include "Scheduler.h"

namespace PAL
{
	/// <summary>
	/// The core application runtime for PAL
	/// </summary>
	class Application
	{
	public:
		/// <summary>
		/// Initialize a new instance of the Application class
		/// </summary>
		Application(std::unique_ptr<Process>&& mainProcess, size_t workerCount);

		/// <summary>
		/// Run
		/// </summary>
		void Run();

		/// <summary>
		/// Print Logs
		/// </summary>
		void PrintLogs(std::chrono::high_resolution_clock::time_point referenceTime);

		/// <summary>
		/// Generate Graph
		/// </summary>
		void GenerateGraph(const std::wstring& filename);

		/// <summary>
		/// Get a value indicating whether the process is completed or not
		/// </summary>
		bool IsProcessCompleted();

	private:
		/// <summary>
		/// The mutex
		/// </summary>
		Scheduler m_scheduler;
	};

} // PAL
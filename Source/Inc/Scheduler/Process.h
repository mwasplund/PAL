// <copyright file="Process.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once
#include "DependencyChain.h"
#include "PendingJobGroupRef.h"
#include "Fence.h"
#include "JobGroup.h"

namespace PAL
{
	/// <summary>
	/// The Process
	/// </summary>
	class Process
	{
	public:
		/// <summary>
		/// Initialize a new instance of the Process class
		/// </summary>
		Process(uint32_t totalPasses);

		/// <summary>
		/// Remove copy constructor and assignment
		/// </summary>
		Process(const Process&) = delete;
		Process& operator=(const Process&) = delete;

		/// <summary>
		/// Get the dependency chain
		/// </summary>
		DependencyChain& GetDependencyChain();

		/// <summary>
		/// Create a work group
		/// </summary>
		JobGroup& CreateJobGroup();

		/// <summary>
		/// Initialize the process
		/// </summary>
		void Initialize();

		/// <summary>
		/// Get a value indicating whether the process is completed or not
		/// </summary>
		bool IsCompleted();

		/// <summary>
		/// Get a value indicating whether the pass is completed or not
		/// </summary>
		bool IsPassCompleted();

		/// <summary>
		/// Get all available jobs
		/// This will check for all completed fences for groups that have not run yet
		/// </summary>
		std::vector<std::shared_ptr<Job>> GetAvailableJobs(Application& app);

		/// <summary>
		/// Assign colors to each work item
		/// </summary>
		std::unordered_map<uint32_t, std::array<unsigned char, 3>> AssignJobColors();

		/// <summary>
		/// Force completed
		/// </summary>
		void ForceCompleted();

	private:
		/// <summary>
		/// Get the work group
		/// </summary>
		JobGroup& GetJobGroup(uint32_t id);

		/// <summary>
		/// Move To Next Pass
		/// </summary>
		void MoveToNextPass();

		/// <summary>
		/// A value indicating whether the process was forced completed
		/// </summary>
		bool _forceCompleted;

		/// <summary>
		/// The total number of passes
		/// </summary>
		uint32_t _totalPasses;

		/// <summary>
		/// The current pass number
		/// </summary>
		uint32_t _currentPass;

		/// <summary>
		/// The mutex
		/// </summary>
		std::mutex _mutex;

		/// <summary>
		/// The dependency chain
		/// </summary>
		DependencyChain _dependencyChain;

		/// <summary>
		/// The work groups
		/// </summary>
		std::vector<std::unique_ptr<JobGroup>> _jobGroups;

		/// <summary>
		/// The active work groups
		/// </summary>
		std::vector<PendingJobGroupRef> _activeJobGroups;
	};

} // PAL
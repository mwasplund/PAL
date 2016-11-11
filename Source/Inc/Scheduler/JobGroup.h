// <copyright file="JobGroup.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once
#include "Job.h"
#include "Fence.h"

namespace PAL
{
	/// <summary>
	/// The work group
	/// </summary>
	class JobGroup
	{
	private:
		/// <summary>
		/// Unique Id for all work groups
		/// </summary>
		static uint32_t _uniqueId;

	public:
		/// <summary>
		/// Initialize a new instance of the JobGroup class
		/// </summary>
		JobGroup();

		/// <summary>
		/// Initialize a new instance of the JobGroup class
		/// </summary>
		JobGroup(JobGroup&& other);

		/// <summary>
		/// Move Assignment operator
		/// </summary>
		void operator=(JobGroup&& other);

		/// <summary>
		/// Remove copy constructor and assignment
		/// </summary>
		JobGroup(JobGroup&) = delete;
		JobGroup& operator=(JobGroup&) = delete;

		/// <summary>
		/// Get the id of the work group
		/// </summary>
		uint32_t GetId() const;

		/// <summary>
		/// Get the jobs
		/// </summary>
		const std::vector<std::shared_ptr<Job>>& GetJobs() const;

		/// <summary>
		/// Create a job
		/// </summary>
		Job& CreateJob(std::function<void(void)> function, bool runOnMainThread = false);

		/// <summary>
		/// Get the fence
		/// </summary>
		Fence& GetFence();

	private:
		/// <summary>
		/// The work group id
		/// </summary>
		uint32_t _id;

		/// <summary>
		/// The work items
		/// </summary>
		std::vector<std::shared_ptr<Job>> _jobs;

		/// <summary>
		/// The fence for this group
		/// </summary>
		Fence _fence;
	};

} // PAL

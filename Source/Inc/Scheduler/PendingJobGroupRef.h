// <copyright file="PendingJobGroupRef.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once

namespace PAL
{
	/// <summary>
	/// The Pending Job Group Reference
	/// </summary>
	class PendingJobGroupRef
	{
	public:
		/// <summary>
		/// Initialize a new instance of the PendingJobGroupRef class
		/// </summary>
		PendingJobGroupRef(JobGroup& jobGroup, std::vector<Fence*>&& dependencies);

		/// <summary>
		/// Get the job group
		/// </summary>
		JobGroup& GetJobGroup();

		/// <summary>
		/// Return a value indicating if the job group is blocked
		/// </summary>
		bool Blocked() const;

		/// <summary>
		/// Set scheduled
		/// </summary>
		void SetScheduled();

		/// <summary>
		/// Return a value indicating if the job group has been scheduled
		/// </summary>
		bool Scheduled() const;

		/// <summary>
		/// Reset the job group and fence
		/// </summary>
		void Reset();

	private:
		/// <summary>
		/// Is scheduled
		/// </summary>
		bool _scheduled;

		/// <summary>
		/// The job group
		/// </summary>
		JobGroup& _jobGroup;

		/// <summary>
		/// The dependecies
		/// </summary>
		std::vector<Fence*> _dependencies;
	};

} // PAL

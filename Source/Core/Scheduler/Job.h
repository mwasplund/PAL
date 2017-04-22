// <copyright file="Job.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once

namespace PAL
{
	/// <summary>
	/// The job
	/// </summary>
	class Job
	{
	public:
		/// <summary>
		/// Initialies a new instance of a Job class
		/// </summary>
		Job(std::function<void(void)>&& function, bool runOnMainThread = false, Fence* fence = nullptr);

		/// <summary>
		/// Remove copy constructor and assignment
		/// </summary>
		Job(const Job&) = delete;
		Job& operator=(const Job&) = delete;

		/// <summary>
		/// Finalizes an instance of a Job class
		/// </summary>
		~Job();

		/// <summary>
		/// Get the id of the work item
		/// </summary>
		uint32_t GetId() const;

		/// <summary>
		/// Get a value indicating whether to run on main thread
		/// </summary>
		bool RunOnMainThread() const;

		/// <summary>
		/// Execute the job
		/// </summary>
		void Execute();

	private:
		/// <summary>
		/// The job id
		/// </summary>
		uint32_t m_id;

		/// <summary>
		/// The function
		/// </summary>
		std::function<void(void)> m_function;

		/// <summary>
		/// The fence for the job group containing this job
		/// </summary>
		Fence* m_fence;

		/// <summary>
		/// A value indicating whether the job must run on the main thread or not
		/// </summary>
		bool m_runOnMainThread;
	};

} // PAL

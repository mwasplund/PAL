// <copyright file="Task.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once
#include "SharedTask.h"

namespace PAL
{
	/// <summary>
	/// The task
	/// </summary>
	template<typename TResult>
	class Task
	{
	public:
		/// <summary>
		/// Create a task
		/// </summary>
		static Task<TResult> Create(std::function<TResult(void)>&& function)
		{
			// Create the implemementation task
			std::shared_ptr<SharedTask<TResult>> sharedTask = 
				SharedTask<TResult>::Create(std::move(function));
			return Task<TResult>(std::move(sharedTask));
		}

	private:
		/// <summary>
		/// Initializes a new instance of the Task class
		/// </summary>
		Task(std::shared_ptr<SharedTask<TResult>>&& sharedTask) :
			m_sharedTask(std::move(sharedTask))
		{
		}

	public:
		/// <summary>
		/// Attach a continuation
		/// </summary>
		template<typename TNextResult>
		Task<TNextResult> Then(std::function<TNextResult(Task<TResult>)>&& function)
		{
			std::shared_ptr<SharedTask<TNextResult>> nextShared = m_shared->Then(std::move(function));
			return Task<TNextResult>(nextShared);
		}

	private:
		/// <summary>
		/// The shared task
		/// </summary>
		std::shared_ptr<SharedTask<TResult>> m_sharedTask;
	};
} // PAL

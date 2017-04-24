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
		template<typename TOther>
		friend class Task;

	public:
		/// <summary>
		/// Create a task
		/// </summary>
		static Task<TResult> Create(std::function<TResult(void)>&& function);

	private:
		/// <summary>
		/// Initializes a new instance of the Task class
		/// </summary>
		Task(std::shared_ptr<SharedTaskResult<TResult>>&& sharedTaskResult);

	public:
		/// <summary>
		/// Attach a continuation
		/// </summary>
		template<typename TNextResult>
		Task<TNextResult> Then(
			std::function<TNextResult(Task<TResult>)>&& function);

		/// <summary>
		/// Get the resulting value
		/// </summary>
		const TResult& Get();

	private:
		/// <summary>
		/// The shared task
		/// </summary>
		std::shared_ptr<SharedTaskResult<TResult>> m_sharedTaskResult;
	};

	template<typename TResult>
	/*static*/ Task<TResult> Task<TResult>::Create(std::function<TResult(void)>&& function)
	{
		// Create the implemementation task
		std::shared_ptr<SharedTask<void, TResult>> sharedTask =
			SharedTask<void, TResult>::Create(std::move(function));
		return Task<TResult>(std::move(sharedTask));
	}

	template<typename TResult>
	Task<TResult>::Task(std::shared_ptr<SharedTaskResult<TResult>>&& sharedTaskResult) :
		m_sharedTaskResult(std::move(sharedTaskResult))
	{
	}

	template<typename TResult>
	template<typename TNextResult>
	Task<TNextResult> Task<TResult>::Then(
		std::function<TNextResult(Task<TResult>)>&& function)
	{
		std::shared_ptr<SharedTask<TResult, TNextResult>> nextShared =
			m_sharedTaskResult->Then(std::move(function));
		return Task<TNextResult>(std::move(nextShared));
	}

	template<typename TResult>
	const TResult& Task<TResult>::Get()
	{
		return m_sharedTaskResult->Get();
	}
} // PAL

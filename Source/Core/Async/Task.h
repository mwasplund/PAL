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
		template<typename TOtherResult>
		friend class Task;
		template<typename TOtherResult, typename TOtherParam>
		friend class SharedTask;

	public:
		/// <summary>
		/// Create a task
		/// </summary>
		static Task<TResult> Create(std::function<TResult(void)>&& function);

	private:
		/// <summary>
		/// Initializes a new instance of the Task class
		/// </summary>
		Task(std::shared_ptr<SharedTaskResult<TResult>>&& sharedTaskResult) :
			m_sharedTaskResult(std::move(sharedTaskResult))
		{
		}

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

	/// <summary>
	/// The task
	/// </summary>
	template<>
	class Task<void>
	{
		template<typename TOtherResult>
		friend class Task;
		template<typename TOtherResult, typename TOtherParam>
		friend class SharedTask;

	private:
		/// <summary>
		/// Initializes a new instance of the Task class
		/// </summary>
		Task(std::shared_ptr<SharedTaskResult<void>>&& sharedTaskResult) :
			m_sharedTaskResult(std::move(sharedTaskResult))
		{
		}

	public:
		/// <summary>
		/// Attach a continuation
		/// </summary>
		template<typename TNextResult>
		Task<TNextResult> Then(
			std::function<TNextResult(Task<void>)>&& function);

		/// <summary>
		/// Get the resulting value
		/// </summary>
		void Get();

	private:
		/// <summary>
		/// The shared task
		/// </summary>
		std::shared_ptr<SharedTaskResult<void>> m_sharedTaskResult;
	};

	template<typename TResult>
	/*static*/ Task<TResult> Task<TResult>::Create(std::function<TResult(void)>&& function)
	{
		// Create the implemementation task
		std::shared_ptr<SharedTask<TResult, void>> sharedTask =
			SharedTask<TResult, void>::CreateAndSchedule(std::move(function));
		return Task<TResult>(std::move(sharedTask));
	}

	template<typename TResult>
	template<typename TNextResult>
	Task<TNextResult> Task<TResult>::Then(
		std::function<TNextResult(Task<TResult>)>&& function)
	{
		std::shared_ptr<SharedTask<TNextResult, TResult>> nextShared =
			m_sharedTaskResult->Then(std::move(function));
		return Task<TNextResult>(std::move(nextShared));
	}

	template<typename TResult>
	const TResult& Task<TResult>::Get()
	{
		return m_sharedTaskResult->Get();
	}

} // PAL

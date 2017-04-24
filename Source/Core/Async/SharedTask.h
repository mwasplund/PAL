// <copyright file="SharedTask.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once
#include "Scheduler.h"
#include "Worker.h"
#include "SharedTaskContinuation.h"
#include "SharedTaskResult.h"

namespace PAL
{
	/// <summary>
	/// The task implementation base 
	/// Contains shared implementation that both void and typed results require
	/// </summary>
	template<typename TParam, typename TResult>
	class SharedTask : 
		public SharedTaskResult<TResult>, 
		public SharedTaskContinuation<TParam>
	{
		template<typename TOther>
		friend class Task;
		template<typename TOther>
		friend class SharedTaskResult;

	private:
		/// <summary>
		/// Initializes a new instance of the SharedTask class
		/// </summary>
		SharedTask(std::function<TResult(Task<TParam>)>&& function);

		void Schedule();
		void Execute();

	private:
		std::weak_ptr<SharedTask> m_weakSelf;
		std::function<TResult(Task<TParam>)> m_function;
	};

	/// <summary>
	/// The task implementation base 
	/// Contains shared implementation that both void and typed results require
	/// </summary>
	template<typename TResult>
	class SharedTask<void, TResult> : 
		public SharedTaskResult<TResult>, 
		public SharedTaskContinuation<void>
	{
		template<typename TOther>
		friend class Task;
		template<typename TOther>
		friend class SharedTaskResult;

	private:
		/// <summary>
		/// Create a task
		/// </summary>
		static std::shared_ptr<SharedTask<void, TResult>> _stdcall Create(
			std::function<TResult(void)>&& function);

		/// <summary>
		/// Initializes a new instance of the SharedTask class
		/// </summary>
		SharedTask(
			std::function<TResult(void)>&& function,
			std::shared_ptr<SharedTaskResult<void>>&& prevSharedTaskResult);

		void Schedule();
		void Execute();

	private:
		std::function<TResult(void)> m_function;
	};

	template<typename TParam, typename TResult>
	SharedTask<TParam, TResult>::SharedTask(
		std::function<TResult(Task<TParam>)>&& function,
		std::shared_ptr<SharedTaskResult<TParam>>&& prevSharedTaskResult) :
		SharedTaskContinuation<TParam>(std::move(prevSharedTaskResult)),
		SharedTaskResult<TResult>(),
		m_function(std::move(function))
	{
	}

	template<typename TResult>
	/*static*/ std::shared_ptr<SharedTask<void, TResult>> SharedTask<void, TResult>::Create(
		std::function<TResult(void)>&& function)
	{
		// Create the shared task
		std::shared_ptr<SharedTask<void, TResult>> sharedTask =
			std::shared_ptr<SharedTask<void, TResult>>(
				new SharedTask<void, TResult>(std::move(function)));

		// Take a weak reference on ourself
		sharedTask->m_weakSelf = sharedTask;

		// Schedule the newly created task
		sharedTask->Schedule();

		return sharedTask;
	}

	template<typename TResult>
	SharedTask<void, TResult>::SharedTask(std::function<TResult(void)>&& function) :
		m_function(std::move(function))
	{
	}

	template<typename TResult>
	void SharedTask<void, TResult>::Schedule()
	{
		std::shared_ptr<SharedTaskResult<TResult>> self = m_weakSelf.lock();
		if (self == nullptr)
		{
			throw std::runtime_error("The self reference was invalid.");
		}

		std::shared_ptr<Job> job = std::make_shared<Job>(
			std::bind(&SharedTask::Execute, self));
		Worker::GetActive().GetScheduler().DispatchJob(std::move(job));
	}

	template<typename TResult>
	void SharedTask<void, TResult>::Execute()
	{
		TResult result = m_function();

		{
			// Protect access to the result and status
			std::lock_guard<std::mutex> lock(m_mutex);

			m_result = std::move(result);
			m_isCompleted = true;

			if (m_continuationTask != nullptr)
			{
				m_continuationTask->Schedule();
			}
		}
	}

} // PAL
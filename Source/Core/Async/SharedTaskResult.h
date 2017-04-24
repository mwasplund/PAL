// <copyright file="SharedTaskResult.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once

namespace PAL
{
	template<typename TResult>
	class SharedTaskResult
	{
		template<typename TOther>
		friend class Task;

	protected:
		SharedTaskResult();

		const TResult& Get() const;

		template<typename TNextResult>
		std::shared_ptr<SharedTask<TResult, TNextResult>> Then(
			std::function<TNextResult(Task<TResult>)>&& function);

	protected:
		mutable std::mutex m_mutex;
		std::weak_ptr<SharedTaskResult<TResult>> m_weakSelf;
		std::shared_ptr<SharedTaskContinuation<TResult>> m_continuationTask;
		TResult m_result;
		bool m_isCompleted;
	};

	template<typename TResult>
	SharedTaskResult<TResult>::SharedTaskResult() :
		m_mutex(),
		m_continuationTask(nullptr),
		m_result(),
		m_isCompleted(false)
	{
	}

	template<typename TResult>
	const TResult& SharedTaskResult<TResult>::Get() const
	{
		// Protect access to the result and status
		std::lock_guard<std::mutex> lock(m_mutex);

		if (!m_isCompleted)
		{
			throw std::runtime_error("The task was not completed, cannot get result.");
		}

		return m_result;
	}

	template<typename TResult>
	template<typename TNextResult>
	std::shared_ptr<SharedTask<TResult, TNextResult>> SharedTaskResult<TResult>::Then(
		std::function<TNextResult(Task<TResult>)>&& function)
	{
		std::shared_ptr<SharedTask> self = m_weakSelf.lock();
		if (self == nullptr)
		{
			throw std::runtime_error("The self reference was invalid.");
		}

		// Create the shared task
		std::shared_ptr<SharedTask<TResult, TNextResult>> nextSharedTask =
			std::shared_ptr<SharedTask<TResult, TNextResult>>(
				new SharedTask<TResult, TNextResult>(
					std::move(function),
					self));
		{
			// Protect access to the result and status
			std::lock_guard<std::mutex> lock(m_mutex);

			if (m_continuationTask != nullptr)
			{
				throw std::runtime_error("There is already a continuation attacked.");
			}

			// Store the continuation
			m_continuationTask = nextSharedTask;

			// If this task has already completed schedule the next task
			if (m_isCompleted)
			{
				// TODO Schedule
			}
		}

		return nextSharedTask;
	}

} // PAL
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
		SharedTaskResult() :
			m_mutex(),
			m_continuationTask(nullptr),
			m_result(),
			m_isCompleted(false)
		{
		}

		const TResult& Get() const;

		template<typename TNextResult>
		std::shared_ptr<SharedTask<TNextResult, TResult>> Then(
			std::function<TNextResult(Task<TResult>)>&& function);

	protected:
		mutable std::mutex m_mutex;
		std::weak_ptr<SharedTaskResult<TResult>> m_weakSelfResult;
		std::shared_ptr<ITaskContinuation> m_continuationTask;
		TResult m_result;
		bool m_isCompleted;
	};

	template<>
	class SharedTaskResult<void>
	{
		template<typename TOther>
		friend class Task;

	protected:
		SharedTaskResult() :
			m_mutex(),
			m_continuationTask(nullptr),
			m_isCompleted(false)
		{
		}

		void Get() const;

		template<typename TNextResult>
		std::shared_ptr<SharedTask<TNextResult, void>> Then(
			std::function<TNextResult(Task<void>)>&& function);

	protected:
		mutable std::mutex m_mutex;
		std::weak_ptr<SharedTaskResult<void>> m_weakSelfResult;
		std::shared_ptr<ITaskContinuation> m_continuationTask;
		bool m_isCompleted;
	};

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
	std::shared_ptr<SharedTask<TNextResult, TResult>> SharedTaskResult<TResult>::Then(
		std::function<TNextResult(Task<TResult>)>&& function)
	{
		// Create the shared task
		std::shared_ptr<SharedTask<TNextResult, TResult>> nextSharedTask =
			SharedTask<TNextResult, TResult>::Create(
				std::move(function),
				m_weakSelfResult.lock());
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
				m_continuationTask->Schedule();
			}
		}

		return nextSharedTask;
	}

} // PAL
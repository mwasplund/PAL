// <copyright file="SharedTask.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once
#include "Scheduler.h"
#include "Worker.h"
#include "ITaskContinuation.h"
#include "SharedTaskResult.h"

namespace PAL
{
	/// <summary>
	/// The task implementation base 
	/// Contains shared implementation that both void and typed results require
	/// </summary>
	template<typename TResult, typename TParam>
	class SharedTask : 
		public SharedTaskResult<TResult>,
		public ITaskContinuation
	{
		template<typename TOther>
		friend class Task;
		template<typename TOther>
		friend class SharedTaskResult;

	private:
		/// <summary>
		/// Create a task
		/// </summary>
		static std::shared_ptr<SharedTask<TResult, TParam>> Create(
			std::function<TResult(Task<TParam>)>&& function,
			std::shared_ptr<SharedTaskResult<TParam>>&& prevSharedTaskResult);

		/// <summary>
		/// Initializes a new instance of the SharedTask class
		/// </summary>
		SharedTask(
			std::function<TResult(Task<TParam>)>&& function,
			std::shared_ptr<SharedTaskResult<TParam>>&& prevSharedTaskResult) :
			m_function(std::move(function)),
			m_prevSharedTaskResult(std::move(prevSharedTaskResult))
		{
		}

		virtual void Schedule() override final;
		void Execute();

	private:
		std::weak_ptr<SharedTask<TResult, TParam>> m_weakSelf;
		std::shared_ptr<SharedTaskResult<TParam>> m_prevSharedTaskResult;
		std::function<TResult(Task<TParam>)> m_function;
	};

	/// <summary>
	/// The task implementation base 
	/// Contains shared implementation that both void and typed results require
	/// </summary>
	template<typename TResult>
	class SharedTask<TResult, void> : 
		public SharedTaskResult<TResult>, 
		public ITaskContinuation
	{
		template<typename TOther>
		friend class Task;
		template<typename TOther>
		friend class SharedTaskResult;

	private:
		/// <summary>
		/// Create a task
		/// </summary>
		static std::shared_ptr<SharedTask<TResult, void>> Create(
			std::function<TResult(void)>&& function);
		static std::shared_ptr<SharedTask<TResult, void>> CreateAndSchedule(
			std::function<TResult(void)>&& function);

		/// <summary>
		/// Initializes a new instance of the SharedTask class
		/// </summary>
		SharedTask(std::function<TResult(void)>&& function) :
			m_function(std::move(function))
		{
		}

		virtual void Schedule() override final;
		void Execute();

	private:
		std::weak_ptr<SharedTask<TResult, void>> m_weakSelf;
		std::function<TResult(void)> m_function;
	};

	/// <summary>
	/// The task implementation base 
	/// Contains shared implementation that both void and typed results require
	/// </summary>
	template<typename TParam>
	class SharedTask<void, TParam> :
		public SharedTaskResult<void>,
		public ITaskContinuation
	{
		template<typename TOther>
		friend class Task;
		template<typename TOther>
		friend class SharedTaskResult;

	private:
		/// <summary>
		/// Create a task
		/// </summary>
		static std::shared_ptr<SharedTask<void, TParam>> Create(
			std::function<void(Task<TParam>)>&& function,
			std::shared_ptr<SharedTaskResult<TParam>>&& prevSharedTaskResult);

		/// <summary>
		/// Initializes a new instance of the SharedTask class
		/// </summary>
		SharedTask(
			std::function<void(Task<TParam>)>&& function,
			std::shared_ptr<SharedTaskResult<TParam>>&& prevSharedTaskResult) :
			m_function(std::move(function)),
			m_prevSharedTaskResult(std::move(prevSharedTaskResult))
		{
		}

		virtual void Schedule() override final;
		void Execute();

	private:
		std::weak_ptr<SharedTask<void, TParam>> m_weakSelf;
		std::shared_ptr<SharedTaskResult<TParam>> m_prevSharedTaskResult;
		std::function<void(Task<TParam>)> m_function;
	};

	template<typename TResult, typename TParam>
	/*static*/ std::shared_ptr<SharedTask<TResult, TParam>> SharedTask<TResult, TParam>::Create(
		std::function<TResult(Task<TParam>)>&& function,
		std::shared_ptr<SharedTaskResult<TParam>>&& prevSharedTaskResult)
	{
		// Create the shared task
		std::shared_ptr<SharedTask<TResult, TParam>> sharedTask =
			std::shared_ptr<SharedTask<TResult, TParam>>(
				new SharedTask<TResult, TParam>(
					std::move(function),
					std::move(prevSharedTaskResult)));

		// Take a weak reference on ourself
		sharedTask->m_weakSelf = sharedTask;
		sharedTask->m_weakSelfResult = sharedTask;

		return sharedTask;
	}

	template<typename TResult>
	/*static*/ std::shared_ptr<SharedTask<TResult, void>> SharedTask<TResult, void>::Create(
		std::function<TResult(void)>&& function)
	{
		// Create the shared task
		std::shared_ptr<SharedTask<TResult, void>> sharedTask =
			std::shared_ptr<SharedTask<TResult, void>>(
				new SharedTask<TResult, void>(std::move(function)));

		// Take a weak reference on ourself
		sharedTask->m_weakSelf = sharedTask;
		sharedTask->m_weakSelfResult = sharedTask;

		return sharedTask;
	}

	template<typename TParam>
	/*static*/ std::shared_ptr<SharedTask<void, TParam>> SharedTask<void, TParam>::Create(
		std::function<void(Task<TParam>)>&& function,
		std::shared_ptr<SharedTaskResult<TParam>>&& prevSharedTaskResult)
	{
		// Create the shared task
		std::shared_ptr<SharedTask<void, TParam>> sharedTask =
			std::shared_ptr<SharedTask<void, TParam>>(
				new SharedTask<void, TParam>(
					std::move(function),
					std::move(prevSharedTaskResult)));

		// Take a weak reference on ourself
		sharedTask->m_weakSelf = sharedTask;
		sharedTask->m_weakSelfResult = sharedTask;

		return sharedTask;
	}

	template<typename TResult>
	/*static*/ std::shared_ptr<SharedTask<TResult, void>> SharedTask<TResult, void>::CreateAndSchedule(
		std::function<TResult(void)>&& function)
	{
		// Create the shared task
		std::shared_ptr<SharedTask<TResult, void>> sharedTask = 
			Create(std::move(function));

		// Schedule the newly created task
		sharedTask->Schedule();

		return sharedTask;
	}

	template<typename TResult, typename TParam>
	void SharedTask<TResult, TParam>::Schedule()
	{
		std::shared_ptr<SharedTask<TResult, TParam>> self = m_weakSelf.lock();
		if (self == nullptr)
		{
			throw std::runtime_error("The self reference was invalid.");
		}

		std::shared_ptr<Job> job = std::make_shared<Job>(
			std::bind(&SharedTask::Execute, self));
		Worker::GetActive().GetScheduler().DispatchJob(std::move(job));
	}

	template<typename TResult>
	void SharedTask<TResult, void>::Schedule()
	{
		std::shared_ptr<SharedTask<TResult, void>> self = m_weakSelf.lock();
		if (self == nullptr)
		{
			throw std::runtime_error("The self reference was invalid.");
		}

		std::shared_ptr<Job> job = std::make_shared<Job>(
			std::bind(&SharedTask::Execute, self));
		Worker::GetActive().GetScheduler().DispatchJob(std::move(job));
	}

	template<typename TParam>
	void SharedTask<void, TParam>::Schedule()
	{
		std::shared_ptr<SharedTask<void, TParam>> self = m_weakSelf.lock();
		if (self == nullptr)
		{
			throw std::runtime_error("The self reference was invalid.");
		}

		std::shared_ptr<Job> job = std::make_shared<Job>(
			std::bind(&SharedTask::Execute, self));
		Worker::GetActive().GetScheduler().DispatchJob(std::move(job));
	}

	template<typename TResult, typename TParam>
	void SharedTask<TResult, TParam>::Execute()
	{
		// Pass along the previous task
		Task<TParam> prevTask(std::move(m_prevSharedTaskResult));
		TResult result = m_function(std::move(prevTask));

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

	template<typename TResult>
	void SharedTask<TResult, void>::Execute()
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

	template<typename TParam>
	void SharedTask<void, TParam>::Execute()
	{
		// Pass along the previous task
		Task<TParam> prevTask(std::move(m_prevSharedTaskResult));
		m_function(std::move(prevTask));

		{
			// Protect access to the result and status
			std::lock_guard<std::mutex> lock(m_mutex);

			m_isCompleted = true;

			if (m_continuationTask != nullptr)
			{
				m_continuationTask->Schedule();
			}
		}
	}

} // PAL
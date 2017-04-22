// <copyright file="SharedTask.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once
#include "Worker.h"
#include "Scheduler.h"

namespace PAL
{
	//template<typename TResult>
	//class Task;
	template<typename TResult>
	class SharedTask;

	/// <summary>
	/// The task implementation
	/// </summary>
	//template<typename TParam>
	//class IContinuationTaskImpl
	//{
	//public:
	//	/// <summary>
	//	/// Schedule the continuation
	//	/// </summary>
	//	virtual void Schedule() = 0;
	//};

	/// <summary>
	/// The task implementation base 
	/// Contains shared implementation that both void and typed results require
	/// </summary>
	template<typename TResult>
	class SharedTaskCommon
	{
	protected:
		/// <summary>
		/// Initializes a new instance of the SharedTask class
		/// </summary>
		SharedTaskCommon() :
			m_mutex(),
			m_isDone(false)//,
			//m_continuation(nullptr)
		{
		}

		/// <summary>
		/// On Done
		/// </summary>
		void OnDone()
		{
			{
				std::lock_guard<std::mutex> lock(m_mutex);

				// Set Done
				m_isDone = true;

				// Dispatch the continuations
				//if (m_continuation != nullptr)
				//{
				//	m_continuation->Schedule();

				//	// Release the continuation
				//	m_continuation = nullptr;
				//}
			}
		}

	protected:
		/// <summary>
		/// A weak reference to self to allow for binding a shared reference
		/// </summary>
		std::weak_ptr<SharedTask<TResult>> m_weakSelf;

		/// <summary>
		/// The mutex
		/// </summary>
		std::mutex m_mutex;

		/// <summary>
		/// A value indicating whether the task has completed or not
		/// </summary>
		bool m_isDone;

		/// <summary>
		/// The continuation task
		/// </summary>
		// std::shared_ptr<IContinuationTaskImpl<TResult>> m_continuation;
	};

	/// <summary>
	/// The task implementation
	/// </summary>
	template<typename TResult>
	class SharedTask : public SharedTaskCommon<TResult>
	{
		friend class Task<TResult>;

	protected:
		/// <summary>
		/// Create a task
		/// </summary>
		static std::shared_ptr<SharedTask<TResult>> Create(std::function<TResult(void)>&& function)
		{
			// Create the shared task
			std::shared_ptr<SharedTask<TResult>> task =
				std::shared_ptr<SharedTask<TResult>>(new SharedTask<TResult>(std::move(function)));

			// Take a weak reference on ourself
			task->m_weakSelf = task;

			// Schedule the newly created task
			task->Schedule();

			return task;
		}

		/// <summary>
		/// Initializes a new instance of the SharedTask class
		/// </summary>
		SharedTask(std::function<TResult(void)>&& function) :
			SharedTaskCommon(),
			m_function(std::move(function)),
			m_result()
		{
		}

		/// <summary>
		/// Schedule the task
		/// </summary>
		void Schedule()
		{
			// Get the shared reference to this task
			std::shared_ptr<SharedTask<TResult>> self = m_weakSelf.lock();
			if (self == nullptr)
			{
				throw std::invalid_argument("Our self weak reference was invalid.");
			}

			// Create the job for this task and dispatch it
			std::shared_ptr<Job> job = std::make_shared<Job>(std::bind(&SharedTask<TResult>::Execute, self));
			Worker::GetActive().GetScheduler().DispatchJob(std::move(job));
		}

		/// <summary>
		/// Execute
		/// </summary>
		void Execute()
		{
			// Invoke the function and notify
			m_result = m_function();

			// Clear the function to release the parent if present
			m_function = nullptr;

			// Notify done
			OnDone();
		}

		/// <summary>
		/// Attach a continuation
		/// </summary>
		template<typename TNextResult>
		std::shared_ptr<SharedTask<TNextResult>> Then(
			std::function<TNextResult(Task<TResult>)> function)
		{
			// Get a shared reference to this task
			std::shared_ptr<SharedTask<TResult>> self = _weakSelf.lock();
			if (self == nullptr)
			{
				throw std::invalid_argument("Our self weak reference was invalid.");
			}

			// Bind the parent task to the continuation
			std::function<TNextResult(void)> continuationFunction =
				std::bind(function, Task<TResult>(self));

			// Create the task implementation
			std::shared_ptr<SharedTask<TNextResult, TResult>> nextTask =
				SharedTask<TNextResult, TResult>::Create(move(continuationFunction));

			{
				// lock to check if the result is set
				std::lock_guard<std::mutex> lock(m_mutex);

				// Save the continuation
				if (m_continuation != nullptr)
				{
					throw std::runtime_error("Already have a continuation attached.");
				}
				else
				{
					m_continuation = nextTask;
				}

				// If the task has already completed then dispatch it
				if (m_isDone)
				{
					// Schedule the continuations
					nextTask->Schedule();
				}
			}

			return nextTask;
		}

	private:
		/// <summary>
		/// The function
		/// </summary>
		std::function<TResult(void)> m_function;

		/// <summary>
		/// The result
		/// </summary>
		TResult m_result;
	};

	/// <summary>
	/// The task implementation
	/// The void specific implementation that does not require a return result
	/// </summary>
	template<>
	class SharedTask<void> : public SharedTaskCommon<void>
	{
	protected:
		/// <summary>
		/// Initializes a new instance of the SharedTask class
		/// </summary>
		SharedTask(std::function<void(void)>&& function) :
			SharedTaskCommon(),
			m_function(std::move(function))
		{
		}

		/// <summary>
		/// Attach a continuation
		/// </summary>
		template<typename TNextResult>
		std::shared_ptr<SharedTask<TNextResult>> Then(std::function<TNextResult(void)>&& function)
		{
			// Create the shared task
			auto sharedTask = SharedTask<TNextResult>::Create(std::move(function));

			{
				// lock to check if the result is set
				std::lock_guard<std::mutex> lock(m_mutex);

				// Save the continuation
				//if (_continuation != nullptr)
				//{
				//	throw std::runtime_error("Already have a continuation attached.");
				//}
				//else
				//{
				//	_continuation = sharedTask;
				//}

				// If the task has already completed then schedule it
				if (m_isReady)
				{
					// Schedule the continuations
					sharedTask->Schedule();
				}
			}
		}

		/// <summary>
		/// Execute
		/// </summary>
		void Execute()
		{
			// Invoke the function and notify
			m_function();

			// Clear the function to release the parent if present
			m_function = nullptr;

			// Notify done
			OnDone();
		}

	private:
		/// <summary>
		/// The function
		/// </summary>
		std::function<void(void)> m_function;
	};

	/// <summary>
	/// The task continuation implementation
	/// Extends the basic task implementation to allow for continuation parameters
	/// </summary>
	//template<typename TResult, typename TParam>
	//class ContinuationTaskImpl : public TaskImpl<TResult>, public IContinuationTaskImpl<TParam>
	//{
	//public:
	//	/// <summary>
	//	/// Create and initialize the ContinuationTaskImpl
	//	/// </summary>
	//	static std::shared_ptr<ContinuationTaskImpl<TResult, TParam>> Create(std::function<TResult(void)>&& function)
	//	{


	//		return taskImpl;
	//	}

	//public:
	//	/// <summary>
	//	/// Initializes a new instance of the ContinuationTaskImpl class
	//	/// </summary>
	//	ContinuationTaskImpl(std::function<TResult(void)>&& function) :
	//		TaskImpl(std::move(function))
	//	{
	//	}
	//};

} // PAL
// <copyright file="TaskImpl.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once
#include "Worker.h"
#include "Application.h"

namespace PAL
{
	template<typename TResult>
	class Task;
	template<typename TResult, typename TParam>
	class ContinuationTaskImpl;

	/// <summary>
	/// The task implementation
	/// </summary>
	template<typename TParam>
	class IContinuationTaskImpl
	{
	public:
		/// <summary>
		/// Schedule the continuation
		/// </summary>
		virtual void Schedule() = 0;
	};

	/// <summary>
	/// The task implementation base 
	/// Contains shared implementation that both void and typed results require
	/// </summary>
	template<typename TResult>
	class TaskImplBase
	{
		// Give all other task implementations access
		template<typename T>
		friend class TaskImplBase;

	protected:
		/// <summary>
		/// Initializes a new instance of the TaskImpl class
		/// </summary>
		TaskImplBase() :
			_mutex(),
			_conditionVar(),
			_isReady(false),
			_continuation(nullptr)
		{
		}

	public:

		/// <summary>
		/// Wait for the task to be run
		/// </summary>
		void Wait()
		{
			// lock to set result
			std::unique_lock<std::mutex> lock(_mutex);

			while (!_isReady)
			{
				_conditionVar.wait(lock);
			}
		}

	protected:
		/// <summary>
		/// Set Ready Notify
		/// </summary>
		void SetReadyNotify()
		{
			{
				std::lock_guard<std::mutex> lock(_mutex);

				// Set ready
				_isReady = true;

				// Dispatch the continuations
				if (_continuation != nullptr)
				{
					_continuation->Schedule();

					// Release the continuation
					_continuation = nullptr;
				}
			}

			// Tell any threads waiting to check themselves
			_conditionVar.notify_all();
		}

	protected:
		/// <summary>
		/// A weak reference to self to allow for binding a shared reference
		/// </summary>
		std::weak_ptr<TaskImpl<TResult>> _weakSelf;

		/// <summary>
		/// The mutex
		/// </summary>
		std::mutex _mutex;

		/// <summary>
		/// The condition variable
		/// </summary>
		std::condition_variable _conditionVar; 

		/// <summary>
		/// A value indicating whether the result is ready
		/// </summary>
		bool _isReady;

		/// <summary>
		/// The continuation task
		/// </summary>
		std::shared_ptr<IContinuationTaskImpl<TResult>> _continuation;
	};

	/// <summary>
	/// The task implementation
	/// </summary>
	template<typename TResult>
	class TaskImpl : public TaskImplBase<TResult>
	{
	public:
		/// <summary>
		/// Create a task
		/// </summary>
		static std::shared_ptr<TaskImpl<TResult>> CreateNew(std::function<TResult(void)> function)
		{
			// Create the task implementation and dispatch it
			auto taskImpl = ContinuationTaskImpl<TResult, void>::Create(move(function));

			// Schedule the newly created task
			taskImpl->Schedule();

			return taskImpl;
		}

	protected:
		/// <summary>
		/// Initializes a new instance of the TaskImpl class
		/// </summary>
		TaskImpl(std::function<TResult(void)>&& function) :
			TaskImplBase(),
			_function(std::move(function)),
			_result()
		{
		}

	public:
		/// <summary>
		/// Get the result of the task
		/// Guaranteed to not block inside a continuation
		/// </summary>
		const TResult& Get()
		{
			// Wait for the result and return it
			Wait();
			return _result;
		}

		/// <summary>
		/// Attach a continuation
		/// </summary>
		template<typename TNextResult>
		std::shared_ptr<TaskImpl<TNextResult>> Then(
			std::function<TNextResult(Task<TResult>)> function)
		{
			// Get a shared reference to this task
			std::shared_ptr<TaskImpl<TResult>> self = _weakSelf.lock();
			if (self == nullptr)
			{
				throw std::invalid_argument("Our self weak reference was invalid.");
			}

			// Bind the parent task to the continuation
			std::function<TNextResult(void)> continuationFunction =
				std::bind(function, Task<TResult>(self));

			// Create the task implementation
			auto taskImpl = ContinuationTaskImpl<TNextResult, TResult>::Create(move(continuationFunction));

			{
				// lock to check if the result is set
				std::lock_guard<std::mutex> lock(_mutex);

				// Save the continuation
				if (_continuation != nullptr)
				{
					throw std::runtime_error("Already have a continuation attached.");
				}
				else
				{
					_continuation = taskImpl;
				}

				// If the task has already completed then dispatch it
				if (_isReady)
				{
					// Schedule the continuations
					taskImpl->Schedule();
				}
			}

			return taskImpl;
		}

		/// <summary>
		/// Execute
		/// </summary>
		void Execute()
		{
			// Invoke the function and notify
			_result = _function();
			SetReadyNotify();

			// Clear the function to release the parent if present
			_function = 0;
		}

	private:
		/// <summary>
		/// The function
		/// </summary>
		std::function<TResult(void)> _function;

		/// <summary>
		/// The result
		/// </summary>
		TResult _result;
	};

	/// <summary>
	/// The task implementation
	/// The void specific implementation that does not require a return result
	/// </summary>
	template<>
	class TaskImpl<void> : public TaskImplBase<void>
	{
	protected:
		/// <summary>
		/// Initializes a new instance of the TaskImpl class
		/// </summary>
		TaskImpl(std::function<void(void)>&& function) :
			TaskImplBase(),
			_function(std::move(function))
		{
		}

	public:
		/// <summary>
		/// Get the result of the task
		/// Guaranteed to not block inside a continuation
		/// </summary>
		void Get()
		{
			// Simply wait
			Wait();
		}

		/// <summary>
		/// Attach a continuation
		/// </summary>
		template<typename TNextResult>
		std::shared_ptr<TaskImpl<TNextResult>> Then(std::function<TNextResult(void)> function)
		{
			// Create the task implementation
			auto taskImpl = ContinuationTaskImpl<TNextResult, void>>::Create(move(function));

			{
				// lock to check if the result is set
				std::lock_guard<std::mutex> lock(_mutex);

				// Save the continuation
				if (_continuation != nullptr)
				{
					throw std::runtime_error("Already have a continuation attached.");
				}
				else
				{
					_continuation = taskImpl;
				}

				// If the task has already completed then schedule it
				if (_isReady)
				{
					// Schedule the continuations
					taskImpl->Schedule();
				}
			}
		}

		/// <summary>
		/// Execute
		/// </summary>
		void Execute()
		{
			// Invoke the function and notify
			_function();
			SetReadyNotify();

			// Clear the function to release the parent if present
			_function = 0;
		}

	private:
		/// <summary>
		/// The function
		/// </summary>
		std::function<void(void)> _function;
	};

	/// <summary>
	/// The task continuation implementation
	/// Extends the basic task implementation to allow for continuation parameters
	/// </summary>
	template<typename TResult, typename TParam>
	class ContinuationTaskImpl : public TaskImpl<TResult>, public IContinuationTaskImpl<TParam>
	{
	public:
		/// <summary>
		/// Create and initialize the ContinuationTaskImpl
		/// </summary>
		static std::shared_ptr<ContinuationTaskImpl<TResult, TParam>> Create(std::function<TResult(void)>&& function)
		{
			// Create the task implementation and dispatch it
			std::shared_ptr<ContinuationTaskImpl<TResult, TParam>> taskImpl =
				std::make_shared<ContinuationTaskImpl<TResult, TParam>>(move(function));

			// Take a weak reference on ourself
			taskImpl->_weakSelf = taskImpl;

			return taskImpl;
		}

	public:
		/// <summary>
		/// Initializes a new instance of the ContinuationTaskImpl class
		/// </summary>
		ContinuationTaskImpl(std::function<TResult(void)>&& function) :
			TaskImpl(std::move(function))
		{
		}

		/// <summary>
		/// Schedule the continuation
		/// </summary>
		virtual void Schedule() override final
		{
			// Get the shared reference to this task
			std::shared_ptr<TaskImpl<TResult>> self = _weakSelf.lock();
			if (self == nullptr)
			{
				throw std::invalid_argument("Our self weak reference was invalid.");
			}

			// Create the job for this task and dispatch it
			std::shared_ptr<Job> job = std::make_shared<Job>(std::bind(&TaskImpl<TResult>::Execute, self));
			Worker::GetActive().GetApp().DispatchJob(job);
		}
	};

} // PAL
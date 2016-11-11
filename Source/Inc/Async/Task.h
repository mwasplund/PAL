// <copyright file="Task.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once
#include "TaskImpl.h"

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
		static Task<TResult> Create(std::function<TResult(void)> function)
		{
			// Create the implemementation task
			std::shared_ptr<TaskImpl<TResult>> impl = TaskImpl<TResult>::CreateNew(move(function));
			return Task<TResult>(impl);
		}

	public:
		/// <summary>
		/// Initializes a new instance of the Task class
		/// </summary>
		Task(std::shared_ptr<TaskImpl<TResult>> pimpl) :
			_pimpl(move(pimpl))
		{
		}

		/// <summary>
		/// Get the result of the task
		/// Guaranteed to not block inside a continuation
		/// </summary>
		const TResult& Get()
		{
			return _pimpl->Get();
		}

		/// <summary>
		/// Attach a continuation
		/// </summary>
		template<typename TNextResult>
		Task<TNextResult> operator>>(std::function<TNextResult(Task<TResult>)> function)
		{
			std::shared_ptr<TaskImpl<TNextResult>> impl = _pimpl->Then(function);
			return Task<TNextResult>(impl);
		}

	private:
		/// <summary>
		/// The implementation
		/// </summary>
		std::shared_ptr<TaskImpl<TResult>> _pimpl;
	};

	template<>
	class Task<void>
	{
	public:
		/// <summary>
		/// Initializes a new instance of the Task class
		/// </summary>
		Task(std::shared_ptr<TaskImpl<void>> pimpl) :
			_pimpl(move(pimpl))
		{
		}

		/// <summary>
		/// Get the result of the task
		/// Guaranteed to not block inside a continuation
		/// </summary>
		void Get()
		{
			return _pimpl->Get();
		}

		/// <summary>
		/// Attach a continuation
		/// </summary>
		template<typename TNextResult>
		Task<TNextResult> Then(std::function<TNextResult(void)> function)
		{
			return Task<TNextResult>(_pimpl->Then(function));
		}

	private:
		/// <summary>
		/// The implementation
		/// </summary>
		std::shared_ptr<TaskImpl<void>> _pimpl;
	};

} // PAL

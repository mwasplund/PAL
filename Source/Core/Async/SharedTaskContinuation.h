// <copyright file="SharedTaskContinuation.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once

namespace PAL
{
	template<typename TParam>
	class SharedTaskContinuation
	{
		template<typename TOtherParam, typename TOtherResult>
		friend class SharedTask;

	private:
		SharedTaskContinuation(
			std::shared_ptr<SharedTaskResult<TParam>>&& prevSharedTaskResult);

		void Schedule();

	private:
		std::shared_ptr<SharedTaskResult<TParam>> m_prevSharedTaskResult;
	};

	template<typename TParam>
	SharedTaskContinuation<TParam>::SharedTaskContinuation(
		std::shared_ptr<SharedTaskResult<TParam>>&& prevSharedTaskResult) :
		m_prevSharedTaskResult(std::move(prevSharedTaskResult))
	{
	}

	template<typename TParam>
	void SharedTaskContinuation<TParam>::Schedule()
	{
	}

	template<>
	void SharedTaskContinuation<void>::Schedule()
	{
	}

} // PAL
// <copyright file="PendingJobGroupRef.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "PendingJobGroupRef.h"

#include "Fence.h"
#include "JobGroup.h"

namespace PAL
{
	PendingJobGroupRef::PendingJobGroupRef(JobGroup& jobGroup, std::vector<Fence*>&& dependencies) :
		m_scheduled(false),
		m_jobGroup(jobGroup),
		m_dependencies(std::move(dependencies))
	{
	}

	JobGroup& PendingJobGroupRef::GetJobGroup()
	{
		return m_jobGroup;
	}

	bool PendingJobGroupRef::Blocked() const
	{
		for (const auto& fence : m_dependencies)
		{
			if (!fence->Completed())
			{
				return true;
			}
		}

		return false;
	}

	void PendingJobGroupRef::SetScheduled()
	{
		m_scheduled = true;
	}

	bool PendingJobGroupRef::Scheduled() const
	{
		return m_scheduled;
	}

	void PendingJobGroupRef::Reset()
	{
		m_scheduled = false;
		m_jobGroup.GetFence().Reset();
	}

} // PAL

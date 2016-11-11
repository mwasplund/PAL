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
		_scheduled(false),
		_jobGroup(jobGroup),
		_dependencies(move(dependencies))
	{
	}

	JobGroup& PendingJobGroupRef::GetJobGroup()
	{
		return _jobGroup;
	}

	bool PendingJobGroupRef::Blocked() const
	{
		for (const auto& fence : _dependencies)
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
		_scheduled = true;
	}

	bool PendingJobGroupRef::Scheduled() const
	{
		return _scheduled;
	}

	void PendingJobGroupRef::Reset()
	{
		_scheduled = false;
		_jobGroup.GetFence().Reset();
	}

} // PAL

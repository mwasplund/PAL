// <copyright file="JobGroup.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "JobGroup.h"

namespace PAL
{
	uint32_t JobGroup::_uniqueId = 1;

	JobGroup::JobGroup() :
		_id(_uniqueId++),
		_jobs(),
		_fence()
	{
	}

	JobGroup::JobGroup(JobGroup&& other) :
		_id(std::move(other._id)),
		_jobs(std::move(other._jobs))
	{
	}

	void JobGroup::operator=(JobGroup&& other)
	{
		_id = std::move(other._id);
		_jobs = std::move(other._jobs);
	}

	uint32_t JobGroup::GetId() const
	{
		return _id;
	}

	const std::vector<std::shared_ptr<Job>>& JobGroup::GetJobs() const
	{
		return _jobs;
	}

	Job& JobGroup::CreateJob(std::function<void(void)> function, bool runOnMainThread)
	{
		// Create the job around the fence for this group
		std::shared_ptr<Job> job = std::make_shared<Job>(function, runOnMainThread, &_fence);

		// Insert the job
		_jobs.push_back(job);

		// Update the fence
		_fence.SetJobCount(static_cast<unsigned int>(_jobs.size()));

		// Return a reference to the job
		return *job;
	}

	Fence& JobGroup::GetFence()
	{
		return _fence;
	}

} // PAL

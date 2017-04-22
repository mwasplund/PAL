// <copyright file="JobGroup.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "JobGroup.h"

namespace PAL
{
	JobGroup::JobGroup() :
		m_jobs(),
		m_fence()
	{
		static uint32_t s_uniqueId = 0;
		m_id = s_uniqueId++;
	}

	JobGroup::JobGroup(JobGroup&& other) :
		m_id(std::move(other.m_id)),
		m_jobs(std::move(other.m_jobs))
	{
	}

	void JobGroup::operator=(JobGroup&& other)
	{
		m_id = std::move(other.m_id);
		m_jobs = std::move(other.m_jobs);
	}

	uint32_t JobGroup::GetId() const
	{
		return m_id;
	}

	const std::vector<std::shared_ptr<Job>>& JobGroup::GetJobs() const
	{
		return m_jobs;
	}

	Job& JobGroup::CreateJob(std::function<void(void)>&& function, bool runOnMainThread)
	{
		// Create the job around the fence for this group
		std::shared_ptr<Job> job =
			std::make_shared<Job>(std::move(function), runOnMainThread, &m_fence);

		// Insert the job
		m_jobs.push_back(job);

		// Update the fence
		m_fence.SetJobCount(static_cast<unsigned int>(m_jobs.size()));

		// Return a reference to the job
		return *job;
	}

	Fence& JobGroup::GetFence()
	{
		return m_fence;
	}

} // PAL

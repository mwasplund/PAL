// <copyright file="Process.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "Process.h"
#include "Job.h"

namespace PAL
{
	Process::Process(uint32_t totalPasses) :
		m_forceCompleted(false),
		m_totalPasses(totalPasses),
		m_currentPass(1),
		m_mutex(),
		m_dependencyChain(),
		m_jobGroups(),
		m_activeJobGroups()
	{
	}

	DependencyChain& Process::GetDependencyChain()
	{
		return m_dependencyChain;
	}

	JobGroup& Process::CreateJobGroup()
	{
		m_jobGroups.push_back(std::make_unique<JobGroup>());
		return *m_jobGroups[m_jobGroups.size() - 1];
	}

	void Process::Initialize()
	{
		// lock to ensure we are the only ones touching the jobs
		std::lock_guard<std::mutex> lock(m_mutex);

		// Add every group to the pending list and map dependencies to parent fences
		for (auto& jobGroup : m_jobGroups)
		{
			std::vector<Fence*> dependencies;

			// Check every edge for dependencies
			for (const auto& edge : m_dependencyChain.GetDependecies())
			{
				// If we are the child add the parent fence as a dependecy
				if (edge.second == jobGroup->GetId())
				{
					// Find the group that the out job is in
					JobGroup& outGroup = GetJobGroup(edge.first);

					dependencies.push_back(&outGroup.GetFence());
				}
			}

			m_activeJobGroups.emplace_back(*jobGroup, move(dependencies));
		}
	}

	bool Process::IsCompleted()
	{
		if (m_forceCompleted)
		{
			return true;
		}

		// Total passes of zero means run forever
		if (m_totalPasses != 0)
		{
			if (m_totalPasses == m_currentPass)
			{
				return IsPassCompleted();
			}
		}

		return false;
	}

	bool Process::IsPassCompleted()
	{
		// Check if all fences are done
		for (const auto& JobGroup : m_jobGroups)
		{
			if (!JobGroup->GetFence().Completed())
			{
				// Found a group that is not done
				return false;
			}
		}

		return true;
	}

	std::vector<std::shared_ptr<Job>> Process::GetAvailableJobs(Scheduler& scheduler)
	{
		// lock to ensure we are the only ones touching the jobs
		std::lock_guard<std::mutex> lock(m_mutex);

		std::vector<std::shared_ptr<Job>> result;

		if (IsPassCompleted() && !IsCompleted())
		{
			MoveToNextPass();
		}

		// Find all the pending job groups that are no longer blocked
		for (auto& jobGroup : m_activeJobGroups)
		{
			if (!jobGroup.Blocked() && !jobGroup.Scheduled())
			{
				const auto& jobs = jobGroup.GetJobGroup().GetJobs();

				for (const auto& job : jobs)
				{
					result.push_back(job);
				}

				jobGroup.SetScheduled();
			}
		}

		return result;
	}

	std::unordered_map<uint32_t, std::array<unsigned char, 3>> Process::AssignJobColors()
	{
		// Collection of available colors
		static const std::array<std::array<float, 3>, 6> colors =
		{ {
			{ 1.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f },
			{ 1.0f, 1.0f, 0.0f },
			{ 0.0f, 1.0f, 1.0f },
			{ 1.0f, 0.0f, 1.0f },
		} };

		std::unordered_map<uint32_t, std::array<unsigned char, 3>> result;

		// Pick similar colors for each group
		size_t colorIndex = 0;
		for (const auto& jobGroup : m_jobGroups)
		{
			std::array<float, 3> groupColor = colors[colorIndex];
			colorIndex = (colorIndex + 1) % colors.size();

			// Scale up the color for each job
			const std::vector<std::shared_ptr<Job>>& jobs = jobGroup->GetJobs();
			for (size_t i = 0; i < jobs.size(); i++)
			{
				// Pick a gradient value 
				float percent = rand() / static_cast<float>(RAND_MAX);
				float gradient = 0.1f + (percent * 0.9f);

				std::array<unsigned char, 3> JobColor;
				JobColor[0] = static_cast<unsigned char>(255 * groupColor[0] * gradient);
				JobColor[1] = static_cast<unsigned char>(255 * groupColor[1] * gradient);
				JobColor[2] = static_cast<unsigned char>(255 * groupColor[2] * gradient);

				result.emplace(jobs[i]->GetId(), JobColor);
			}
		}

		return result;
	}

	void Process::ForceCompleted()
	{
		// lock to ensure we are the only ones touching the work items
		std::lock_guard<std::mutex> lock(m_mutex);

		m_forceCompleted = true;
	}

	JobGroup& Process::GetJobGroup(uint32_t id)
	{
		for (auto& JobGroup : m_jobGroups)
		{
			if (JobGroup->GetId() == id)
			{
				return *JobGroup;
			}
		}

		throw std::invalid_argument("Could not find the requested work group.");
	}

	void Process::MoveToNextPass()
	{
		// Reset all work groups
		for (auto& JobGroup : m_activeJobGroups)
		{
			JobGroup.Reset();
		}

		// Increment the pass count
		m_currentPass++;
	}

} // PAL
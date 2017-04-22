// <copyright file="Application.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "Application.h"
#include "EventLogParser.h"
#include "EventLogGraph.h"
#include "Job.h"

namespace PAL
{
	Application::Application(std::unique_ptr<Process>&& mainProcess, size_t workerCount) :
		m_scheduler(std::move(mainProcess), workerCount)
	{
	}

	void Application::Run()
	{
		m_scheduler.Run();
	}

	bool Application::IsProcessCompleted()
	{
		return m_scheduler.IsProcessCompleted();
	}

	void Application::PrintLogs(std::chrono::high_resolution_clock::time_point referenceTime)
	{
		m_scheduler.PrintLogs(referenceTime);
	}

	void Application::GenerateGraph(const std::wstring& filename)
	{
		m_scheduler.GenerateGraph(filename);
	}

} // PAL
// <copyright file="Main.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"

/// <summary>
/// The root of all evil
/// </summary>
int main()
{
	std::chrono::high_resolution_clock::time_point programStartTime = std::chrono::high_resolution_clock::now();

	std::unique_ptr<PAL::Process> process = std::make_unique<PAL::Process>(1);
	PAL::JobGroup& mainGroup = process->CreateJobGroup();
	PAL::Job& job = mainGroup.CreateJob([]()
	{
		std::wcout << "Open File..." << std::endl;

		std::function<void(PAL::Task<std::shared_ptr<PAL::IFile>>)> func = 
			[](PAL::Task<std::shared_ptr<PAL::IFile>> task)
			{
				std::shared_ptr<PAL::IFile> file = task.Get();
			};

		PAL::IFile::GetFileFromPathAsync(L"TestFile.txt") >> func;

	}, true);

	std::wcout << L"Run Application..." << std::endl;

	PAL::Application app(std::move(process), 4);
	app.Run();

	std::wcout << L"Done Application." << std::endl;

	std::wcout << L"Create Graph..." << std::endl;

	// Print out the log
	// app.PrintLogs(programStartTime);
	app.GenerateGraph(L"ScheduledGraph.bmp");

	std::wcout << L"Done!" << std::endl;

	return 0;
}

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
		std::wcout << L"Open File..." << std::endl;

		PAL::IFile::GetFileFromPathAsync(L"TestFile.txt").Then<void>(
			[](PAL::Task<std::shared_ptr<PAL::IFile>> task)
			{
				std::wcout << L"File Opened." << std::endl;
				std::shared_ptr<PAL::IFile> file = task.Get();
			});

		int value = PAL::Task<int>::Create(
			[]() -> int
			{
				return 1 + 1;
			}).Then<int>(
				[](PAL::Task<int> result) -> int
				{
					return result.Get() * 5;
				}).Get();

		value++;
	}, true);

	std::wcout << L"Run Application..." << std::endl;

	PAL::Application app(std::move(process), 4);
	app.Run();

	std::wcout << L"Application Done!" << std::endl;

	std::wcout << L"Create Graph..." << std::endl;

	// Print out the log
	// app.PrintLogs(programStartTime);
	app.GenerateGraph(L"ScheduledGraph.bmp");

	std::wcout << L"Done!" << std::endl;

	return 0;
}

// <copyright file="MainWorker.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "MainWorker.h"
#include "Scheduler.h"

namespace PAL
{
	MainWorker::MainWorker(Scheduler& scheduler) :
		Worker(scheduler)
	{
	}

	bool MainWorker::IsRunning()
	{
		return !_scheduler.IsProcessCompleted();
	}

} // PAL
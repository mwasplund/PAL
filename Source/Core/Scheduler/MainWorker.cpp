// <copyright file="MainWorker.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "MainWorker.h"
#include "Application.h"

namespace PAL
{
	MainWorker::MainWorker(Application& app) :
		Worker(app)
	{
	}

	bool MainWorker::IsRunning()
	{
		return !_app.IsProcessCompleted();
	}

} // PAL
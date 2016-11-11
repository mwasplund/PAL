// <copyright file="MainWorker.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once
#include "Worker.h"

namespace PAL
{
	/// <summary>
	/// The main worker
	/// </summary>
	class MainWorker : public Worker
	{
	public:
		/// <summary>
		/// Initialize a new instance of the MainWorker class
		/// </summary>
		MainWorker(Application& app);

	protected:
		/// <summary>
		/// Is Running
		/// </summary>
		virtual bool IsRunning() override final;
	};

} // PAL
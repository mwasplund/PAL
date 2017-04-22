// <copyright file="Worker.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once
#include "Worker.h"

namespace PAL
{
	/// <summary>
	/// The thread worker
	/// </summary>
	class ThreadWorker : public Worker
	{
	public:
		/// <summary>
		/// Initialize a new instance of the ThreadWorker class
		/// </summary>
		ThreadWorker(Scheduler& app);

		/// <summary>
		/// Initialize
		/// </summary>
		void Initialize();

		/// <summary>
		/// Shutdown
		/// </summary>
		void Shutdown();

		/// <summary>
		/// Wait Join
		/// </summary>
		void WaitJoin();

	protected:
		/// <summary>
		/// Is Running
		/// </summary>
		virtual bool IsRunning() override final;

	private:
		/// <summary>
		/// A value indicating if the worker is running
		/// </summary>
		bool m_running;

		/// <summary>
		/// The thread
		/// </summary>
		std::unique_ptr<std::thread> m_thread;
	};

} // PAL
// <copyright file="EventLogger.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once
#include "EventLog.h"

namespace PAL
{
	/// <summary>
	/// The event logger
	/// </summary>
	class EventLogger
	{
	public:
		/// <summary>
		/// Initializes a new instance of the EventLogger class
		/// </summary>
		EventLogger(); 

		/// <summary>
		/// Log an event
		/// </summary>
		void LogEvent(EventType type, uint32_t id);

		/// <summary>
		/// Get the log
		/// </summary>
		const EventLog& GetLog();

	private:
		/// <summary>
		/// The event log
		/// </summary>
		EventLog _log;
	};

} // PAL

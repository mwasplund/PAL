// <copyright file="EventLogParser.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once
#include "EventLog.h"

namespace PAL
{
	/// <summary>
	/// The event logger
	/// </summary>
	class EventLogParser
	{
	public:
		/// <summary>
		/// Initializes a new instance of the EventLogParser class
		/// </summary>
		EventLogParser();

		/// <summary>
		/// Parse log
		/// </summary>
		void Parse(
			std::chrono::high_resolution_clock::time_point referenceTime, 
			const EventLog& log);

	private:
		/// <summary>
		/// Parse entry
		/// </summary>
		void ParseEntry(
			std::chrono::high_resolution_clock::time_point referenceTime, 
			const Event& entry);

		/// <summary>
		/// Parse event type
		/// </summary>
		const std::wstring& ParseEventType(EventType type);
	};

} // PAL

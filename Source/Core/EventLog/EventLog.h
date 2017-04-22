// <copyright file="EventLog.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once

namespace PAL
{
	/// <summary>
	/// The event
	/// </summary>
	typedef std::tuple<EventType, uint32_t, std::chrono::high_resolution_clock::time_point> Event;

	/// <summary>
	/// The event log
	/// </summary>
	typedef std::vector<Event> EventLog;

} // PAL

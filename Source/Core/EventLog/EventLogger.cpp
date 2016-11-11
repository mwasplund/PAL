// <copyright file="EventLogger.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "EventLogger.h"

namespace PAL
{
	EventLogger::EventLogger()
	{
		_log.reserve(1024 * 32);
	}

	void EventLogger::LogEvent(EventType type, uint32_t id)
	{
		_log.emplace_back(type, id, std::chrono::high_resolution_clock::now());
	}

	const EventLog& EventLogger::GetLog()
	{
		return _log;
	}

} // PAL

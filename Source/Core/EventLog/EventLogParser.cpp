// <copyright file="EventLogParser.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "EventLogParser.h"
#include "EventType.h"

namespace PAL
{
	EventLogParser::EventLogParser()
	{
	}

	void EventLogParser::Parse(
		std::chrono::high_resolution_clock::time_point referenceTime,
		const EventLog& log)
	{
		for (const auto& entry : log)
		{
			ParseEntry(referenceTime, entry);
		}
	}

	void EventLogParser::ParseEntry(
		std::chrono::high_resolution_clock::time_point referenceTime,
		const Event& entry)
	{
		typedef std::chrono::duration<int, std::ratio_multiply<std::chrono::hours::period, std::ratio<24>>::type> days;

		// Write out the timestamp
		std::chrono::high_resolution_clock::time_point timestamp = std::get<2>(entry);
		std::chrono::high_resolution_clock::duration time = timestamp - referenceTime;
		days totalDays = std::chrono::duration_cast<days>(time);
		time -= totalDays;
		std::chrono::hours totalHours = std::chrono::duration_cast<std::chrono::hours>(time);
		time -= totalHours;
		std::chrono::minutes totalMinutes = std::chrono::duration_cast<std::chrono::minutes>(time);
		time -= totalMinutes;
		std::chrono::seconds totalSeconds = std::chrono::duration_cast<std::chrono::seconds>(time);
		time -= totalSeconds;
		std::chrono::milliseconds totalMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(time);
		time -= totalMilliseconds;
		std::chrono::microseconds totalMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(time);
		time -= totalMicroseconds;

		std::wcout << L"[" << totalHours.count() << L":";
		std::wcout << totalMinutes.count() << L":";
		std::wcout << totalSeconds.count() << L".";
		std::wcout << totalMilliseconds.count() << L".";
		std::wcout << totalMicroseconds.count() << L"] ";

		// Write the event type and id
		std::wcout << ParseEventType(std::get<0>(entry)) << L" " << std::get<1>(entry) << std::endl;
	}

	const std::wstring& EventLogParser::ParseEventType(EventType type)
	{
		switch (type)
		{
			case PAL::EventType::WorkerBegin:
			{
				static const std::wstring value = L"WorkerBegin";
				return value;
			}
			case PAL::EventType::WorkerExecuteJobBegin:
			{
				static const std::wstring value = L"WorkerExecuteJobBegin";
				return value;
			}
			case PAL::EventType::WorkerExecuteJobEnd:
			{
				static const std::wstring value = L"WorkerExecuteJobEnd";
				return value;
			}
			case PAL::EventType::WorkerEnd:
			{
				static const std::wstring value = L"WorkerEnd";
				return value;
			}
			case PAL::EventType::JobBegin:
			{
				static const std::wstring value = L"JobBegin";
				return value;
			}
			case PAL::EventType::JobEnd:
			{
				static const std::wstring value = L"JobEnd";
				return value;
			}
			default:
			{
				static const std::wstring value = L"UnknownEvent";
				return value;
			}
		}
	}

} // PAL
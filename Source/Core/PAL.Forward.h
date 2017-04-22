// <copyright file="PAL.Forward.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once

namespace PAL
{
	// Async 
	class CancellationToken;
	class IFile;
	class IStream;
	template<typename TResult>
	class Task;
	template<typename TResult>
	class SharedTask;

	// EventLogger
	class EventLogger;
	class EventLogGraph;
	class EventLogParser;
	enum class EventType;

	// Process
	class DependencyChain;
	class Fence;
	class PendingJobGroupRef;
	class Process;
	class JobGroup;
	class Job;

	// Scheduler
	class MainWorker;
	class Scheduler;
	class Application;
	class ThreadWorker;
	class Worker;

} // PAL
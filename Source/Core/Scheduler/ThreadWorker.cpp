// <copyright file="ThreadWorker.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "ThreadWorker.h"

#ifdef _WIN32
#include <windows.h>

const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void SetThreadName(std::thread& thread, const char* threadName)
{
	DWORD winThreadId = ::GetThreadId(static_cast<HANDLE>(thread.native_handle()));

	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = winThreadId;
	info.dwFlags = 0;

	__try
	{
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*) &info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}
#else
void SetThreadName(std::thread& thread, const char* threadName)
{
}
#endif

namespace PAL
{
	ThreadWorker::ThreadWorker(Scheduler& scheduler) :
		Worker(scheduler),
		m_running(false)
	{
	}

	void ThreadWorker::Initialize()
	{
		m_running = true;
		m_thread = std::make_unique<std::thread>(std::bind(&Worker::MainLoop, this));

#ifdef _DEBUG
		std::string threadName = "Worker #";
		threadName += std::to_string(m_id);
		SetThreadName(*m_thread, threadName.c_str());
#endif
	}

	void ThreadWorker::Shutdown()
	{
		m_running = false;
	}

	void ThreadWorker::WaitJoin()
	{
		m_thread->join();
		m_thread = nullptr;
	}

	bool ThreadWorker::IsRunning()
	{
		return m_running;
	}

} // PAL
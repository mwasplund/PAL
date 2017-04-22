// <copyright file="Fence.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "Fence.h"

namespace PAL
{
	Fence::Fence() :
		m_initialJobCount(0),
		m_remainingJobCount(0)
	{
	}

	void Fence::SetJobCount(unsigned int value)
	{
		m_initialJobCount = value;
		m_remainingJobCount = value;
	}

	bool Fence::Signal()
	{
		// Decrement the atomic integer, only one thread will ever see zero remaining
		unsigned int remaining = m_remainingJobCount--;
		bool completed = (remaining == 1);
		return completed;
	}

	bool Fence::Completed() const
	{
		return m_remainingJobCount == 0;
	}

	void Fence::Reset()
	{
		m_remainingJobCount = m_initialJobCount;
	}

} // PAL

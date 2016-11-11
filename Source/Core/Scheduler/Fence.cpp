// <copyright file="Fence.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "Fence.h"

namespace PAL
{
	Fence::Fence() :
		_initialJobCount(0),
		_remainingJobCount(0)
	{
	}

	void Fence::SetJobCount(unsigned int value)
	{
		_initialJobCount = value;
		_remainingJobCount = value;
	}

	bool Fence::Signal()
	{
		// Decrement the atomic integer, only one thread will ever see zero remaining
		unsigned int remaining = _remainingJobCount--;
		bool completed = (remaining == 1);
		return completed;
	}

	bool Fence::Completed() const
	{
		return _remainingJobCount == 0;
	}

	void Fence::Reset()
	{
		_remainingJobCount = _initialJobCount;
	}

} // PAL

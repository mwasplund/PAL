// <copyright file="SharedTaskContinuation.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once

namespace PAL
{
	class ITaskContinuation
	{
	public:
		virtual void Schedule() = 0;
	};

} // PAL
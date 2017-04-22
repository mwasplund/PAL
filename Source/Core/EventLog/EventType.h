// <copyright file="EventType.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once

namespace PAL
{
	/// <summary>
	/// The event type
	/// </summary>
	enum class EventType
	{
		/// <summary>
		/// Worker Begin Event
		/// </summary>
		WorkerBegin,

		/// <summary>
		/// Worker Execute Job Begin Event
		/// </summary>
		WorkerExecuteJobBegin,

		/// <summary>
		/// Worker Execute Job End Event
		/// </summary>
		WorkerExecuteJobEnd,

		/// <summary>
		/// Worker End Event
		/// </summary>
		WorkerEnd,

		/// <summary>
		/// Job Begin Event
		/// </summary>
		JobBegin,

		/// <summary>
		/// Job End Event
		/// </summary>
		JobEnd,
	};

} // PAL

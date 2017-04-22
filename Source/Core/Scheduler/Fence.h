// <copyright file="Fence.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once

namespace PAL
{
	/// <summary>
	/// The fence
	/// </summary>
	class Fence
	{
	public:
		/// <summary>
		/// Initialize a new instance of the Fence class
		/// </summary>
		Fence();

		/// <summary>
		/// Set job count
		/// </summary>
		void SetJobCount(unsigned int value);

		/// <summary>
		/// Signal Job Completed
		/// </summary>
		bool Signal();

		/// <summary>
		/// Check if all jobs completed
		/// </summary>
		bool Completed() const;

		/// <summary>
		/// Reset the fence
		/// </summary>
		void Reset();

	private:
		/// <summary>
		/// The initial job count
		/// </summary>
		unsigned int m_initialJobCount;

		/// <summary>
		/// The remaining job count
		/// </summary>
		std::atomic_uint m_remainingJobCount;
	};

} // PAL

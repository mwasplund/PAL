// <copyright file="IStream.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once

namespace PAL
{
	/// <summary>
	/// The async stream interface
	/// </summary>
	class IStream
	{
	public:
		/// <summary>
		/// Read the stream asynchronously
		/// </summary>
		virtual Task<std::wstring> ReadAsync() = 0;
	};

} // PAL

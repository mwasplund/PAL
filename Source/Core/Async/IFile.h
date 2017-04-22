// <copyright file="IFile.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once

namespace PAL
{
	/// <summary>
	/// The async file interface
	/// </summary>
	class IFile
	{
	public:
		/// <summary>
		/// Get a file from the provided path
		/// </summary>
		static Task<std::shared_ptr<IFile>> GetFileFromPathAsync(const std::wstring& path);

	public:
		/// <summary>
		/// Open the file asynchronously
		/// </summary>
		virtual Task<std::shared_ptr<IStream>> OpenAsync() = 0;
	};

} // PAL
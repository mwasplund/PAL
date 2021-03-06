// <copyright file="File.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once
#include <IFile.h>

namespace PAL { namespace Win32
{
	/// <summary>
	/// The async file
	/// </summary>
	class File : public IFile
	{
		friend Task<std::shared_ptr<IFile>> IFile::GetFileFromPathAsync(const std::wstring& path);
		friend class FileStream;

	public:
		/// <summary>
		/// Initializes a new instance of the File class
		/// </summary>
		File(std::wstring path, HANDLE handle);

		/// <summary>
		/// Open the file asynchronously
		/// </summary>
		virtual Task<std::shared_ptr<IStream>> OpenAsync() override final;

	private:
		std::weak_ptr<File> m_self;
		std::wstring m_path;
		HANDLE m_handle;
	};
} } // PAL

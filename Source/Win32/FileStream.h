// <copyright file="FileStream.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once
#include <IStream.h>
#include "File.h"

namespace PAL { namespace Win32
{
	/// <summary>
	/// The async file stream
	/// </summary>
	class FileStream : public IStream
	{
	public:
		static std::shared_ptr<FileStream> Create(std::shared_ptr<File> file);

	private:
		/// <summary>
		/// Initializes a new instance of the File class
		/// </summary>
		FileStream(std::shared_ptr<File> file);

	public:
		/// <summary>
		/// Read the stream asynchronously
		/// </summary>
		virtual Task<std::wstring> ReadAsync() override final;

	private:
		/// <summary>
		/// On Read Completed
		/// </summary>
		void OnReadCompleted(
			DWORD dwErrorCode,
			DWORD dwNumberOfBytesTransfered,
			LPOVERLAPPED lpOverlapped) noexcept;

	private:
		OVERLAPPED m_overlapped;
		std::shared_ptr<File> m_file;
		std::weak_ptr<FileStream> m_weakSelf;
		std::vector<char> m_buffer;
	};
} } // PAL

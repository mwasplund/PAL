// <copyright file="File.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "File.h"
#include "Job.h"
#include "Task.h"
#include "FileStream.h"

using namespace PAL;
using namespace PAL::Win32;

Task<std::shared_ptr<IFile>> PAL::IFile::GetFileFromPathAsync(const std::wstring& path)
{
	std::wstring pathCopy = path;
	return Task<std::shared_ptr<IFile>>::Create(
		[pathCopy]() -> std::shared_ptr<File>
	{
		DWORD shareMode = 0;

		HANDLE handle = ::CreateFileW(
			pathCopy.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			nullptr,
			OPEN_ALWAYS,
			FILE_FLAG_OVERLAPPED,
			0);

		std::shared_ptr<File> result = nullptr;
		if (handle != INVALID_HANDLE_VALUE)
		{
			result = std::make_shared<File>(std::move(pathCopy), handle);
			result->m_self = result;
		}
		else
		{
			DWORD error = GetLastError();
		}

		return result;
	});
}


File::File(std::wstring path, HANDLE handle) :
	m_path(std::move(path)),
	m_handle(handle)
{
}

Task<std::shared_ptr<PAL::IStream>> File::OpenAsync()
{
	std::shared_ptr<File> self = m_self.lock();
	return Task<std::shared_ptr<IStream>>::Create([self]()
	{
		return FileStream::Create(self);
	});
}

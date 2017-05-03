// <copyright file="FileStream.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "FileStream.h"
#include "Task.h"

using namespace PAL;
using namespace PAL::Win32;

/*static*/ std::shared_ptr<FileStream> FileStream::Create(std::shared_ptr<File> file)
{
	std::shared_ptr<FileStream> result = 
		std::shared_ptr<FileStream>(new FileStream(std::move(file)));

	// Capture a reference to self
	result->m_weakSelf = result;

	return result;
}

FileStream::FileStream(std::shared_ptr<File> file) :
	m_file(std::move(file))
{
}

Task<std::wstring> FileStream::ReadAsync()
{
	return Task<std::wstring>::Create([]()
	{
		return L"Test String.";
	});
}

void FileStream::OnReadCompleted(
	DWORD /*errorCode*/,
	DWORD /*numberOfBytesTransfered*/,
	LPOVERLAPPED /*overlapped*/) noexcept
{
	// TODO
}
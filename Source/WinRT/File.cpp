// <copyright file="File.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "File.h"
#include "Job.h"
#include "Task.h"
#include "FileStream.h"

using namespace PAL;
using namespace PAL::WinRT;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Storage;

Task<std::shared_ptr<IFile>> PAL::IFile::GetFileFromPathAsync(const std::wstring& path)
{
	String^ pathCopy = ref new String(path.c_str());

	auto asyncOperation = StorageFile::GetFileFromPathAsync(pathCopy);
	asyncOperation->Completed = 
		ref new AsyncOperationCompletedHandler<StorageFile^>(&File::OnGetFileFromPathCompleted);

	return Task<std::shared_ptr<IFile>>::Create(
		[pathCopy]() -> std::shared_ptr<File>
	{
		std::shared_ptr<File> result = nullptr;


		return result;
	});
}

/*static*/ void File::OnGetFileFromPathCompleted(
	IAsyncOperation<StorageFile^>^ asyncOperation,
	AsyncStatus status) noexcept
{

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

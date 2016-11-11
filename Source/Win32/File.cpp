// <copyright file="File.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "File.h"
#include "Job.h"
#include "Task.h"

namespace PAL
{
	Task<std::shared_ptr<IFile>> IFile::GetFileFromPathAsync(const std::wstring& path)
	{
		//HANDLE hFile;
		//char DataBuffer[] = "This is some test data to write to the file.";
		//DWORD dwBytesToWrite = (DWORD) strlen(DataBuffer);
		//DWORD dwBytesWritten = 0;
		//BOOL bErrorFlag = FALSE;

		//HANDLE fh = ::CreateFileW(
		//	name.c_str(), 
		//	dwDesiredAccess,
		//	dwShareMode,
		//	nullptr, 
		//	dwCreationDisposition, 
		//	FILE_FLAG_OVERLAPPED, 
		//	0);

		//hFile = CreateFileW(
		//	argv[1],                // name of the write
		//	GENERIC_WRITE,          // open for writing
		//	0,                      // do not share
		//	NULL,                   // default security
		//	CREATE_NEW,             // create new file only
		//	FILE_ATTRIBUTE_NORMAL,  // normal file
		//	NULL);                  // no attr. template

		//if (hFile == INVALID_HANDLE_VALUE)
		//{
		//	DisplayError(TEXT("CreateFile"));
		//	_tprintf(TEXT("Terminal failure: Unable to open file \"%s\" for write.\n"), argv[1]);
		//	return;
		//}

		//_tprintf(TEXT("Writing %d bytes to %s.\n"), dwBytesToWrite, argv[1]);

		//bErrorFlag = WriteFile(
		//	hFile,           // open file handle
		//	DataBuffer,      // start of data to write
		//	dwBytesToWrite,  // number of bytes to write
		//	&dwBytesWritten, // number of bytes that were written
		//	NULL);            // no overlapped structure

		//if (FALSE == bErrorFlag)
		//{
		//	DisplayError(TEXT("WriteFile"));
		//	printf("Terminal failure: Unable to write to file.\n");
		//}
		//else
		//{
		//	if (dwBytesWritten != dwBytesToWrite)
		//	{
		//		// This is an error because a synchronous write that results in
		//		// success (WriteFile returns TRUE) should write all data as
		//		// requested. This would not necessarily be the case for
		//		// asynchronous writes.
		//		printf("Error: dwBytesWritten != dwBytesToWrite\n");
		//	}
		//	else
		//	{
		//		_tprintf(TEXT("Wrote %d bytes to %s successfully.\n"), dwBytesWritten, argv[1]);
		//	}
		//}

		//CloseHandle(hFile);

		return Task<std::shared_ptr<IFile>>::Create([]()
		{
			return std::make_shared<File>();
		});
	}

	Task<std::shared_ptr<IStream>> File::OpenAsync()
	{
		return Task<std::shared_ptr<IStream>>::Create([]()
		{
			return nullptr;
		});
	}

} // PAL

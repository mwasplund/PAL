// <copyright file="File.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once
#include <IFile.h>

namespace PAL
{
	/// <summary>
	/// The async file
	/// </summary>
	class File : public IFile
	{
	public:
		/// <summary>
		/// Open the file asynchronously
		/// </summary>
		virtual Task<std::shared_ptr<IStream>> OpenAsync() override final;
	};

} // PAL

// <copyright file="EventLogGraph.h" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#pragma once
#include "EventLog.h"

namespace PAL
{
	/// <summary>
	/// The column struct
	/// </summary>
	struct Column
	{
		/// <summary>
		/// The color
		/// </summary>
		unsigned char Color[3];

		/// <summary>
		/// The start of the column [0 - 1]
		/// </summary>
		float Start;

		/// <summary>
		/// The end of the column [0 - 1]
		/// </summary>
		float End;
	};

	/// <summary>
	/// The event log graph
	/// </summary>
	class EventLogGraph
	{
	public:
		/// <summary>
		/// Initializes a new instance of the EventLogGraph class
		/// </summary>
		EventLogGraph();

		/// <summary>
		/// Generate the graph
		/// </summary>
		void Generate(
			const std::wstring& filename,
			const std::vector<const EventLog*>& logs,
			std::unordered_map<uint32_t, std::array<unsigned char, 3>>& JobColors);

	private:
		/// <summary>
		/// Initialize
		/// </summary>
		void Initialize(
			const std::vector<const EventLog*>& logs,
			std::unordered_map<uint32_t, std::array<unsigned char, 3>>& JobColors);

		/// <summary>
		/// Create Bitmap
		/// </summary>
		void CreateBMP(const std::wstring& filename);

		/// <summary>
		/// Get pixel
		/// </summary>
		void GetPixel(int rowIndex, int x, unsigned char pixel[3]);

		/// <summary>
		/// The image width
		/// </summary>
		int _imageWidth;

		/// <summary>
		/// The column height
		/// </summary>
		int _columnHeight;

		/// <summary>
		/// The rows
		/// </summary>
		std::vector<std::vector<Column>> _rows;
	};

} // PAL

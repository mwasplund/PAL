// <copyright file="EventLogGraph.cpp" company="mwasplund"> 
//     Copyright (c) MWASPLUND. All rights reserved.
// </copyright>

#include "pch.h"
#include "EventLogGraph.h"
#include "EventType.h"

namespace PAL
{
	EventLogGraph::EventLogGraph() :
		_columnHeight(40),
		_imageWidth(1000),
		_rows()
	{
	}

	void EventLogGraph::Generate(
		const std::wstring& filename,
		const std::vector<const EventLog*>& logs,
		std::unordered_map<uint32_t, std::array<unsigned char, 3>>& JobColors)
	{
		Initialize(logs, JobColors);
		CreateBMP(filename);
	}

	void EventLogGraph::Initialize(
		const std::vector<const EventLog*>& logs,
		std::unordered_map<uint32_t, std::array<unsigned char, 3>>& JobColors)
	{
		// Find the max and min time
		std::chrono::high_resolution_clock::time_point minTime = std::chrono::high_resolution_clock::time_point::max();
		std::chrono::high_resolution_clock::time_point maxTime = std::chrono::high_resolution_clock::time_point::min();
		for (const EventLog* eventLog : logs)
		{
			minTime = std::min(minTime, std::get<2>(eventLog->front()));
			maxTime = std::max(maxTime, std::get<2>(eventLog->back()));
		}

		// Calculate running time
		std::chrono::high_resolution_clock::duration totalTime = maxTime - minTime;
		long long totalTimeMicro = std::chrono::duration_cast<std::chrono::microseconds>(totalTime).count();
		long long totalTimeMilli = std::chrono::duration_cast<std::chrono::milliseconds>(totalTime).count();

		bool useMirco = totalTimeMicro < 10000;

		_imageWidth = useMirco ? static_cast<int>(totalTimeMicro) : static_cast<int>(totalTimeMilli);

		// Generate the rows of columns
		_rows.clear();
		for (const EventLog* eventLog : logs)
		{
			_rows.emplace_back();
			std::vector<Column>& row = _rows.back();

			// Create a column for each job
			Column* currentColumn = nullptr;
			for (const Event& entry : *eventLog)
			{
				std::chrono::high_resolution_clock::time_point eventTime = std::get<2>(entry);
				float percent;
				if (useMirco)
				{
					long long offsetTimeMicro = std::chrono::duration_cast<std::chrono::microseconds>(eventTime - minTime).count();
					percent = offsetTimeMicro / static_cast<float>(totalTimeMicro);
				}
				else
				{
					long long offsetTimeMilli = std::chrono::duration_cast<std::chrono::milliseconds>(eventTime - minTime).count();
					percent = offsetTimeMilli / static_cast<float>(totalTimeMilli);
				}
				uint32_t id = std::get<1>(entry);
				switch (std::get<0>(entry))
				{
					case EventType::JobBegin:
					{
						// Insert new column
						row.emplace_back();
						currentColumn = &row.back();

						// Set Color
						std::array<unsigned char, 3> color = JobColors[id];
						currentColumn->Color[0] = color[0];
						currentColumn->Color[1] = color[1];
						currentColumn->Color[2] = color[2];

						// Set start percentage
						currentColumn->Start = percent;

						break;
					}
					case EventType::JobEnd:
					{
						// Set start percentage
						currentColumn->End = percent;

						// Done with this column
						currentColumn = nullptr;

						break;
					}
				}
			}
		}
	}

	void EventLogGraph::CreateBMP(const std::wstring& filename)
	{
		int width = _imageWidth;
		int height = _columnHeight * static_cast<int>(_rows.size());

		// Open the file
		std::ofstream file(filename, std::ofstream::binary);

		unsigned char bmpFileHeader[14] = { 0 };
		unsigned char bmpInfoHeader[40] = { 0 };
		unsigned char pixel[3] = { 0 };

		int requiredPadding = (4 - ((width * sizeof(pixel)) % 4)) % 4;
		int infoHeaderSize = sizeof(bmpInfoHeader);
		int planes = 1;
		int bitsPerPixel = 8 * sizeof(pixel);
		int offsetToPixelArray = sizeof(bmpFileHeader) + sizeof(bmpInfoHeader);
		int filesize =
			offsetToPixelArray +
			(sizeof(pixel) * width * height) +
			(requiredPadding * height);
		
		bmpFileHeader[0] = 'B';
		bmpFileHeader[1] = 'M';

		bmpFileHeader[2] = (unsigned char) (filesize);
		bmpFileHeader[3] = (unsigned char) (filesize >> 8);
		bmpFileHeader[4] = (unsigned char) (filesize >> 16);
		bmpFileHeader[5] = (unsigned char) (filesize >> 24);

		bmpFileHeader[10] = (unsigned char) (offsetToPixelArray);
		bmpFileHeader[11] = (unsigned char) (offsetToPixelArray >> 8);
		bmpFileHeader[12] = (unsigned char) (offsetToPixelArray >> 16);
		bmpFileHeader[13] = (unsigned char) (offsetToPixelArray >> 24);

		// Write info header
		bmpInfoHeader[0] = (unsigned char) (infoHeaderSize);
		bmpInfoHeader[1] = (unsigned char) (infoHeaderSize >> 8);
		bmpInfoHeader[2] = (unsigned char) (infoHeaderSize >> 16);
		bmpInfoHeader[3] = (unsigned char) (infoHeaderSize >> 24);

		bmpInfoHeader[4] = (unsigned char) (width);
		bmpInfoHeader[5] = (unsigned char) (width >> 8);
		bmpInfoHeader[6] = (unsigned char) (width >> 16);
		bmpInfoHeader[7] = (unsigned char) (width >> 24);

		bmpInfoHeader[8] = (unsigned char) (height);
		bmpInfoHeader[9] = (unsigned char) (height >> 8);
		bmpInfoHeader[10] = (unsigned char) (height >> 16);
		bmpInfoHeader[11] = (unsigned char) (height >> 24);

		bmpInfoHeader[12] = (unsigned char) (planes);
		bmpInfoHeader[13] = (unsigned char) (planes >> 8);

		bmpInfoHeader[14] = (unsigned char) (bitsPerPixel);
		bmpInfoHeader[15] = (unsigned char) (bitsPerPixel >> 8);

		file.write(reinterpret_cast<const char*>(bmpFileHeader), sizeof(bmpFileHeader));
		file.write(reinterpret_cast<const char*>(bmpInfoHeader), sizeof(bmpInfoHeader));

		// Allocate a single row
		std::vector<unsigned char> row(_imageWidth * sizeof(pixel));

		// Go backward through the rows since BMP go bottom to top
		for (int rowIndex = static_cast<int>(_rows.size() - 1); rowIndex >= 0; rowIndex--)
		{
			// Generate the row for this log
			for (int x = 0; x < width; x++)
			{
				GetPixel(rowIndex, x, pixel);

				// Swap from RGB to BGR
				std::swap(pixel[0], pixel[2]);

				// Save the pixel
				row[x * 3 + 0] = pixel[0];
				row[x * 3 + 1] = pixel[1];
				row[x * 3 + 2] = pixel[2];
			}

			// Write the row for the height of the column
			for (int y = 0; y < _columnHeight; y++)
			{
				// Write the row
				file.write(reinterpret_cast<const char*>(row.data()), row.size());

				// Add padding to multiple of 4
				static const char padding[3] = { 0 };
				file.write(padding, requiredPadding);
			}
		}

		file.close();
	}

	void EventLogGraph::GetPixel(int rowIndex, int x, unsigned char pixel[3])
	{
		// Determine what row we are in
		std::vector<Column>& row = _rows[rowIndex];

		// Determine the percent 
		float percent = x / static_cast<float>(_imageWidth);

		// Find what column we are in
		Column* currentColumn = nullptr;
		for (Column& column : row)
		{
			if (column.Start <= percent && column.End >= percent)
			{
				currentColumn = &column;
				break;
			}
		}

		if (currentColumn != nullptr)
		{
			pixel[0] = currentColumn->Color[0];
			pixel[1] = currentColumn->Color[1];
			pixel[2] = currentColumn->Color[2];
		}
		else
		{
			// Clear the pixel if not in column
			pixel[0] = 255;
			pixel[1] = 255;
			pixel[2] = 255;
		}
	}
} // PAL

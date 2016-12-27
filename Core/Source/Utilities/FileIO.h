/*
===========================================================================
FileIO.h

Aggregates all the basic components of the engine in a single header file.
This file is included by the main header S3DGE.h
===========================================================================
*/

#pragma once

#include <fstream>
#include "../Internal/Log.h"

namespace s3dge
{
	class FileIO
	{
	public:
		static std::string ReadFromFile(cstring filepath)
		{
			std::ifstream ifs(filepath);
			std::string content(std::istreambuf_iterator<char>(ifs.rdbuf()), std::istreambuf_iterator<char>());

			return content;
		}

		static std::string ReadFromFile(const std::string& filepath)
		{
			std::ifstream ifs((filepath).c_str());
			std::string content(std::istreambuf_iterator<char>(ifs.rdbuf()), std::istreambuf_iterator<char>());

			return content;
		}

		static bool CheckFileExists(cstring name) 
		{
			if (FILE *file = fopen(name, "r")) 
			{
				fclose(file);
				return true;
			}
			else 
				return false;
		}

	private:
		FileIO(void);
		FileIO(const FileIO& tRef) = delete;				// Disable copy constructor.
		FileIO& operator = (const FileIO& tRef) = delete;	// Disable assignment operator.
		~FileIO(void) {}
	};
	
}
#ifndef _LOG_HPP
#define _LOG_HPP

#include <ctime>
#include <stdlib.h>
#include <string>
#include <vector>

#include "Debug.hpp"

class Log {
	std::vector<std::string> list;
	size_t maxSize;
	public:
		Log(size_t maxSize);
		void Add(std::string message);
		void Clear();
		std::vector<std::string> GetLines(int quantity, int width, int skipLines);
	private:
		std::vector<std::string> SplitLine(int line, int width);
};

#endif
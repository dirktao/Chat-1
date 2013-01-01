#ifndef _DEBUG_HPP
#define _DEBUG_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

class Debug {
	public:
		static int counter;
		static std::string IntToStr(int i);
		static void AddLine(std::string line, std::string logFile);
		static void AddLine(std::string line);
};

#endif
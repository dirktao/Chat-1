#include "Debug.hpp"

int Debug::counter = 0;

std::string Debug::IntToStr(int i) {
	std::stringstream ss;
	ss << i;
	std::string str;
	ss >> str;
	return str;
}

void Debug::AddLine(std::string line, std::string logFile) {
	std::ofstream file(logFile.c_str(), std::ios::ate|(counter == 0 ? std::ios::trunc : std::ios::app));
	if(file.is_open()) {
		file << ++counter << " " << line << std::endl;
		file.close();
	}
}

void Debug::AddLine(std::string line) {
	AddLine(line, "debuglog.dat");
}
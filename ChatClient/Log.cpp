#include "Log.hpp"

Log::Log(size_t maxSize) {
	this->maxSize = maxSize;
}

void Log::Add(std::string message) {
	if(!message.empty()) {
		std::string action = message.find(" ") == std::string::npos ? message : message.substr(0, message.find(" "));
		message.erase(0, action.size());

		// Prevents from crashing in special situations
		if(message.substr(0, 1) != " ")
			message.insert(0, " ");

		switch(atoi(action.c_str())) {
			case 1:
				message.insert(0, "Connects:");
				break;
			case 2:
				message.insert(0, "Disconnects:");
				break;
			case 3:
				message.erase(0, 1);
				break;
			case 4:
				message.insert(0, "Names:");
				break;
			case 5:
				message.erase(0, 1);
				if(message.find(" ") != std::string::npos)
					message = message.substr(0, message.find(" ")) + " is now known as " + message.substr(message.find(" ") + 1, message.size() - message.rfind(" ") - 1);
				break;
			case 6:
				message.insert(0, "Whois:");
				break;
			case 9:
				message.insert(0, "Unknown command:");
				break;
			case 10:
				message.insert(0, "Operators:");
				break;
			case 11:
				message.erase(0, 1);
				if(message.find(" ") != std::string::npos)
					message = message.substr(0, message.find(" ")) + " is now operator by " + message.substr(message.find(" ") + 1, message.size() - message.find(" ") - 1);
				else
					message += " is now operator";
				break;
			case 12:
				message.erase(0, 1);
				if(message.find(" ") != std::string::npos)
					message = message.substr(0, message.find(" ")) + " was removed from operators by " + message.substr(message.find(" ") + 1, message.size() - message.find(" ") - 1);
				else
					message += " was removed from operators";
				break;
			case 13:
				message.insert(0, "Bans:");
				break;
			case 14:
				message.erase(0, 1);
				if(message.find(" ") != std::string::npos)
					message = message.substr(0, message.find(" ")) + " got banned by " + message.substr(message.find(" ") + 1, message.size() - message.find(" ") - 1);
				else
					message += " got banned";
				break;
			case 15:
				message.erase(0, 1);
				if(message.find(" ") != std::string::npos)
					message = message.substr(0, message.find(" ")) + " was removed from bans by " + message.substr(message.find(" ") + 1, message.size() - message.find(" ") - 1);
				else
					message += " was removed from bans";
				break;
			case 20:
				message.erase(0, 1);
				if(message.find(" ") != std::string::npos)
					message = "Nicknames length must be between" + message.substr(0, message.find(" ")) + "-" + message.substr(message.find(" ") + 1, message.size() - message.find(" ") - 1);
				else
					message = "Invalid nickname length";
				break;
			case 21:
				message = "Special characters aren't allowed in the nickname";
				break;
			case 22:
				message.erase(0, 1);
				message = "Nickname \"" + message + "\" is already in use";
				break;
			case 23:
				message.erase(0, 1);
				message = "User \"" + message + "\" not found";
				break;
			case 24:
				message.erase(0, 1);
				message = "User \"" + message + "\" not found";
				break;
			case 25:
				message.erase(0, 1);
				message = "User \"" + message + "\" not found";
				break;
			case 26:
				message = "Mode has been already set";
				break;
			case 27:
				message = "You're not an operator";
				break;
			case 29:
				message = "You're banned from this server";
				break;
			case 30:
				message = "Initializing connection failed";
				break;
			case 31:
				message = "Getting host by name failed";
				break;
			case 32:
				message = "Connecting to server failed";
				break;
			case 33:
				message = "Connection initializing failed"; // Making socket non-blocking failed
				break;
			case 34:
				message = "Lost connection to server";
				break;
		}

		time_t rawTime;
		time(&rawTime);
		struct tm *timeInfo = localtime(&rawTime);
		char timestamp[9];
		strftime(timestamp, 9, "[%H:%M] ", timeInfo);

		list.push_back(timestamp + message);
		while(list.size() > maxSize && maxSize > 0)
			list.erase(list.begin());
	}
}

void Log::Clear() {
	list.clear();
}

std::vector<std::string> Log::GetLines(int quantity, int width, int skipLines) {
	std::vector<std::string> lines;

	if(skipLines > (int) list.size() - quantity) {
		skipLines = (int) list.size() - quantity;
		if(skipLines > (int) list.size() || skipLines < 0)
			skipLines = 0;
	}

	std::vector<std::string> buffer;
	int firstLine = list.size() - 1 - skipLines;
	int lastLine = list.size() - 1 - skipLines - quantity;
	if(lastLine < 0)
		lastLine = 0;

	for(int line = firstLine; line >= lastLine; --line) {
		buffer.clear();
		buffer.push_back(list[line]);

		while((int) buffer[buffer.size() - 1].size() > width) {
			buffer.push_back(buffer[buffer.size() - 1].substr(width, buffer[buffer.size() - 1].size() - width));
			buffer[buffer.size() - 2] = buffer[buffer.size() - 2].substr(0, width);
		}

		for(int i = buffer.size() - 1; i >= 0; --i)
			lines.push_back(buffer[i]);
	}

	while((int) lines.size() > quantity)
		lines.erase(lines.begin());

	return lines;
}
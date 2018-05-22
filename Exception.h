#pragma once
#include <string>

class Exception {
private:
	std::string message;
	Exception* exception;

public:
	Exception() {
		message = "Unknow reason";
	}

	Exception(std::string message) {
		this->message = message;
	}

	Exception(Exception* exception) {
		this->exception = exception;
	}

	Exception(std::string message, Exception* exception) {
		this->message = message;
		this->exception = exception;
	}

	std::string getMessage() {
		return exception->getMessage() + message;
	}
};
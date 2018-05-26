#pragma once
#include <string>

class Exception {	//проверить как работает класс-исключение
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

	~Exception() {
		if (exception != nullptr) {
			delete exception;
		}
	}

	std::string getMessage() {
		if (exception != nullptr) {
			return exception->getMessage() + message;
		}
		else {
			return message;
		}
	}
};
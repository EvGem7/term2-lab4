#pragma once
#include <string>
#include "Exception.h"

class BitStringException : public Exception {
public:
	BitStringException(std::string message) : Exception(message) {}

	BitStringException(Exception* exception) : Exception(exception) {}

	BitStringException(std::string message, Exception* exception) : Exception(message, exception) {}
};

class BitString {
private:
	std::string bits;

public:
	BitString() {}

	BitString(unsigned char byte) {
		append(byte);
	}

	BitString(std::string str) {
		append(str);
	}

	static BitString toBitString(std::string& str);
	static BitString toBitString(unsigned char byte);
	void addBit(bool bit);
	std::string getBytes(size_t amount);

	std::string getBytes() {
		return getBytes(bits.size() / 8 + 1);
	}

	size_t size() {
		return bits.size();
	}

	void clear() {
		bits.clear();
	}

	void erase(size_t start, size_t amount) {
		bits.erase(start, amount);
	}

	void append(BitString& attachable) {
		bits += attachable.bits;
	}

	void append(std::string& str) {
		append(toBitString(str));
	}

	void append(unsigned char ch) {
		append(toBitString(ch));
	}

	BitString subBitString(size_t start, size_t amount) {
		BitString result;
		result.bits = bits.substr(start, amount);
		return result;
	}

	bool operator [](int i) {
		return bits[i] == '1';
	}
};


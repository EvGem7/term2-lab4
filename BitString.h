#pragma once
#include <string>
#include "Exception.h"

class BitStringException : Exception {
public:
	BitStringException(std::string message) : Exception(message) {}

	BitStringException(Exception* exception) : Exception(exception) {}

	BitStringException(std::string message, Exception* exception) : Exception(message, exception) {}
};

class BitString {
private:
	std::string bits;

public:
	class Iterator {
	private:
		size_t pos;
		BitString& bitString;
	public:
		Iterator(BitString& bitString) : bitString(bitString) {
			pos = 0;
		}

		void move(int delta) {
			if (pos + delta < 0 || pos + delta >= bitString.bits.size()) {
				throw BitStringException("out of array bound");
			}
			pos += delta;
		}

		std::string getBytes(size_t amount) {
			BitString bStr;
			bStr.bits = bitString.bits.substr(pos, bitString.bits.size() - pos);
			return bStr.getBytes(amount);
		}

		std::string getBytes() {
			BitString bStr;
			bStr.bits = bitString.bits.substr(pos, bitString.bits.size() - pos);
			return bStr.getBytes();
		}
	};

	BitString() {}

	BitString(std::string str) {
		append(str);
	}

	std::string getBytes(size_t amount);
	std::string getBytes() {
		return getBytes(bits.size() + 1);
	}

	static BitString toBitString(std::string str);
	static BitString toBitString(unsigned char byte);

	bool operator [](int i) {
		return bits[i] == '1';
	}

	size_t getBitsCount() {
		return bits.size();
	}

	void clear() {
		bits = "";
	}

	void addBit(bool bit) {
		if (bit) {
			bits += "1";
		}
		else {
			bits += "0";
		}
	}
	void append(BitString& attachable) {
		bits += attachable.bits;
	}

	void append(std::string str) {
		append(toBitString(str));
	}

	void append(unsigned char ch) {
		append(toBitString(ch));
	}
};


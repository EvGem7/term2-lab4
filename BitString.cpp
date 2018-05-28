#include "BitString.h"

std::string BitString::getBytes(size_t amount) {
	std::string result;
	for (auto i = 0U; i < bits.size() && i < amount * 8; i += 8) {
		unsigned char byte = 0;
		for (auto j = i, currentBit = 0U; j < i + 8 && j < bits.size(); j++, currentBit++) {
			if (bits[j] == '1') {
				byte |= 1 << (7 - currentBit);
			}
		}
		result += byte;
	}
	return result;
}

BitString BitString::toBitString(unsigned char byte) {
	BitString result;
	for (auto i = 0U; i < 8U; i++) {
		if (byte & (1 << (7 - i))) {
			result.addBit(true);
		}
		else {
			result.addBit(false);
		}
	}
	return result;
}

BitString BitString::toBitString(std::string& str) {
	BitString result;
	for (auto byte : str) {
		result.append(toBitString(byte));
	}
	return result;
}

void BitString::addBit(bool bit) {
	if (bit) {
		bits += "1";
	}
	else {
		bits += "0";
	}
}
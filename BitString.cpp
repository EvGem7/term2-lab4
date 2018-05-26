#include "BitString.h"

std::string BitString::getBytes(size_t amount) {
	std::string result;
	for (int i = 0; i < bits.size() && i < amount * 8; i += 8) {
		unsigned char byte = 0;
		for (int j = i, currentBit = 0; j < i + 8 && j < bits.size(); j++, currentBit++) {
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
	for (int i = 0; i < 8; i++) {
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
	for (char byte : str) {
		result.append(toBitString(byte));
	}
	return result;
}
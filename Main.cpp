#include "Huffman.h"
#include <iostream>
#include <ctime>
//#include <cmath>

void efficientCount(const time_t startTime, char* source, char* destination) {
	double delta = (double)(clock() - startTime) / CLOCKS_PER_SEC;
	std::fstream sourceFile(source, std::ios::binary | std::ios::in);
	std::fstream destinationFile(destination, std::ios::binary | std::ios::in);
	if (!sourceFile.is_open() || !destinationFile.is_open()) {
		throw new Exception("while efficient counting some file couldn't be open");
	}
	sourceFile.seekg(0, sourceFile.end);
	destinationFile.seekg(0, destinationFile.end);
	int sourceSize = sourceFile.tellg();
	int destinationSize = destinationFile.tellg();
	double efficient = abs((double)(sourceSize - destinationSize)) / ((sourceSize > destinationSize)? sourceSize : destinationSize);
	double speed = (double)destinationSize / delta;
	std::cout << "Efficient: " << 100 * efficient << "%" << std::endl;
	std::cout << "Speed: " << speed / 1024 << " kilobytes/sec" << std::endl;
	sourceFile.close();
	destinationFile.close();
}

int main(int argc, char** argv) {
	const clock_t startTime = clock();
	if (argc != 4) {
		std::cout << "Usage: %programName% %key% %sourceFile% %destinationFile%" << std::endl;
		std::cout << "key: code, decode" << std::endl;
		return -1;
	}
	Huffman huffman;
	try {
		switch (argv[1][0]) {
		case 'c':
			huffman.code(argv[2], argv[3]);
			break;
		case 'd':
			huffman.decode(argv[2], argv[3]);
			break;
		default:
			std::cout << "Wrong key. You can use only code and decode keys." << std::endl;
			return -2;
		}
		efficientCount(startTime, argv[2], argv[3]);
	}
	catch (Exception* e) {
		std::cout << e->getMessage();
		delete e;
		return -3;
	}
	return 0;
}
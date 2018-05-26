#include "Huffman.h"
#include "BitString.h"
#include <iostream>

int main(int argc, char** argv) {
	/*if (argc != 4) {
		std::cout << "Usage: %programName% %key% %sourceFile% %destinationFile%" << std::endl;
		std::cout << "key: code, decode" << std::endl;
	}*/
	Huffman huffman;
	try {
		huffman.code("1.txt", "2.txt");
		huffman.decode("2.txt", "3.txt");
		/*switch (argv[1][0]) {
		case 'c':
			huffman.code(argv[2], argv[3]);
			break;
		case 'd':
			huffman.decode(argv[2], argv[3]);
			break;
		default:
			std::cout << "Wrong key. You can use only code and decode keys." << std::endl;
		}*/
	}
	catch (Exception* e) {
		std::cout << e->getMessage();
		delete e;
	}
}
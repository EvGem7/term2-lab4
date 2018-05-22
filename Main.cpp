#include "Huffman.h"
#include "BitString.h"
#include <iostream>

int main(int argc, char** argv) {
	Huffman huffman;
	huffman.code("1.txt", "2.txt");
	huffman.decode("2.txt", "3.txt");
	system("pause");
}
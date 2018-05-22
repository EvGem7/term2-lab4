#pragma once
#include <fstream>
#include <string>
#include "BitString.h"
#include "Exception.h"

class HuffmanException : public Exception {
public:
	HuffmanException(std::string message) : Exception(message) {}

	HuffmanException(Exception* exception) : Exception(exception) {}

	HuffmanException(std::string message, Exception* exception) : Exception(message, exception) {}
};

class Huffman {
private:
	class HuffmanNode {
	private:
		void deleteSubtree(HuffmanNode* node);

	public:
		size_t freq;
		unsigned char byte;
		HuffmanNode *left, *right, *parent;

		HuffmanNode() {
			left = right = parent = nullptr;
		}

		explicit HuffmanNode(HuffmanNode* parent) {
			left = right = nullptr;
			this->parent = parent;
		}

		HuffmanNode(unsigned char byte, size_t freq) {
			this->byte = byte;
			this->freq = freq;
			left = right = parent = nullptr;
		}

		void deleteSubtree() {
			deleteSubtree(this);
		}

		friend bool operator < (const HuffmanNode& node1, const HuffmanNode& node2) { ///////////////??
			return node1.freq > node2.freq;	///////////////??
		}
	};

	std::ifstream fin;
	std::ofstream fout;

	void deepFirstSearch(HuffmanNode* node, BitString& treeStructure, std::string& alphabet, BitString* codingTable, BitString& currentCode);

	void checkFile() {
		if (!fin.is_open() || !fout.is_open()) {
			throw HuffmanException("some file is not opened");
		}
	}

public:
	void setSourceFile(std::string sourceFile);
	void setDestinationFile(std::string destinationFile);

	void code();
	void code(std::string sourceFile, std::string destinationFile);

	void decode();
	void decode(std::string sourceFile, std::string destinationFile);
};

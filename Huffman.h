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

		friend bool operator < (const HuffmanNode& node1, const HuffmanNode& node2) {
			return node1.freq > node2.freq;	// there's > because in priority queue use descending sort
		}
	};

	std::ifstream fin;
	std::ofstream fout;

	/*
	this function is using while coding.
	it codes structure of tree to save it to archive,
	saves alphabet and coding table for every used byte.
	alphabet is being making up in order of deep-first-search.
	*/
	void deepFirstSearch(HuffmanNode* node, BitString& treeStructure, std::string& alphabet, BitString* codingTable, BitString& currentCode);

	/*
	this function is using while decoding.
	it assigns bytes to the corresponding leaves using the alphabet.
	*/
	void assignLeaves(HuffmanNode* node, std::string& alphabet);

	void checkFileOpening() {
		if (!fin.is_open() || !fout.is_open()) {
			throw new HuffmanException("some file cannot be opened");
		}
	}

	void checkFileDecoding() {
		if (!fin.good()) {
			throw new HuffmanException("file is corrupted");
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

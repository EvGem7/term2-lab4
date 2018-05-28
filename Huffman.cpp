#include "Huffman.h"
#include "PriorityQueue.h"
#include <iostream>
#include <cmath>

#define DOWN  1
#define UP    0
#define LEFT  1
#define RIGHT 0
#define LAST_BITS_INFO_SIZE 3

using namespace std;


void Huffman::setSourceFile(string sourceFile) {
	if (fin.is_open()) {
		fin.close();
	}
	fin.open(sourceFile, ios::binary | ios::in);
	if (!fin) {
		throw new HuffmanException("cannot open source file");
	}
}

void Huffman::setDestinationFile(string destinationFile) {
	if (fout.is_open()) {
		fout.close();
	}
	fout.open(destinationFile, ios::binary | ios::out);
	if (!fout) {
		throw new HuffmanException("cannot open destination file");
	}
}

void Huffman::code(std::string sourceFile, std::string destinationFile) {
	setSourceFile(sourceFile);
	setDestinationFile(destinationFile);
	code();
}

void Huffman::decode(std::string sourceFile, std::string destinationFile) {
	setSourceFile(sourceFile);
	setDestinationFile(destinationFile);
	decode();
}

void Huffman::HuffmanNode::deleteSubtree(HuffmanNode* node) {
	if (node->left) {
		deleteSubtree(node->left);
		node->left = nullptr;
		deleteSubtree(node);
	}
	else if (node->right) {
		deleteSubtree(node->right);
		node->right = nullptr;
		deleteSubtree(node);
	}
	else {
		delete node;
	}
}

void Huffman::deepFirstSearch(HuffmanNode* node, BitString& treeStructure, std::string& alphabet, BitString* codingTable, BitString& currentCode = BitString()) {
	if (node->left) {
		treeStructure.addBit(DOWN);
		auto left = currentCode;
		left.addBit(LEFT);
		deepFirstSearch(node->left, treeStructure, alphabet, codingTable, left);
		if (node->right) {
			auto right = currentCode;
			right.addBit(RIGHT);
			deepFirstSearch(node->right, treeStructure, alphabet, codingTable, right);
		}
	}
	else {
		treeStructure.addBit(UP);
		codingTable[node->byte] = currentCode;
		alphabet += node->byte;
	}
}

void Huffman::code() {
	checkFileOpening();
	fin.seekg(0, fin.end);
	const auto FILE_SIZE = (size_t)fin.tellg();
	fin.seekg(0, fin.beg);
	const short BYTE_RANGE = 256;
	size_t freqs[BYTE_RANGE] = {};
	while (!fin.eof()) {
		unsigned char byte;
		fin.read((char*)&byte, 1);
		if (!fin.eof()) {
			freqs[byte]++;
		}
	}

	PriorityQueue<HuffmanNode> queue;
	for (auto i = 0U; i < BYTE_RANGE; i++) {
		if (freqs[i] > 0) {
			queue.push(HuffmanNode(i, freqs[i]));
		}
	}
	while (queue.size() > 1) {	//till coding we needn't use parent pointer
		HuffmanNode newNode;
		newNode.left   = new HuffmanNode();
		newNode.right  = new HuffmanNode();
		*newNode.left  = queue.poll();			
		*newNode.right = queue.poll();			
		newNode.freq   = newNode.left->freq + newNode.right->freq;
		queue.push(newNode);
	}
	auto root = new HuffmanNode();
	*root = queue.poll();
	//we've built Huffman's tree 

	BitString treeStructure;
	BitString codingTable[BYTE_RANGE];
	string alphabet;
	deepFirstSearch(root, treeStructure, alphabet, codingTable);
	if (root->left == nullptr && root->right == nullptr) {
		codingTable[root->byte].addBit(LEFT); //assume the root as left son
	}

	auto compressedSize = 0U;	//in bits
	for (auto i = 0U; i < BYTE_RANGE; i++) {
		compressedSize += freqs[i] * codingTable[i].size();
	}
	compressedSize += treeStructure.size() + 8 * alphabet.size() + LAST_BITS_INFO_SIZE;
	if (ceil(compressedSize / 8.0) >= FILE_SIZE) {
		throw new HuffmanException("there's no sence to compress file");
	}
	BitString lastUnreadableBitsAmount((unsigned char)(8 - compressedSize % 8));
	lastUnreadableBitsAmount.erase(0, 8 - LAST_BITS_INFO_SIZE);
	
	BitString buffer;
	buffer.append(treeStructure);
	buffer.append(alphabet);
	buffer.append(lastUnreadableBitsAmount);
	fin.clear();
	fin.seekg(0, fin.beg);
	while (!fin.eof()) {
		unsigned char byte;
		fin.read((char*)&byte, 1);
		if (!fin.eof()) {
			if (buffer.size() % 8 == 0) {
				fout << buffer.getBytes();
				buffer.clear();
			}
			buffer.append(codingTable[byte]);
		}
	}
	if (buffer.size() > 0) {
		fout << buffer.getBytes();
	}

	fin.close();
	fout.close();
	root->deleteSubtree();
}

void Huffman::decode() {
	checkFileOpening();

	fin.seekg(0, fin.end);
	const auto FILE_SIZE = fin.tellg();
	fin.seekg(0, fin.beg);

	BitString buffer;
	auto root = new HuffmanNode();
	unsigned char byte;

	fin.read((char*)&byte, 1);	
	checkFileDecoding();
	buffer.append(byte);

	size_t alphabetOffset;
	size_t leavesNumber = 0;
	auto currentNode = root;
	for (auto i = 0U; currentNode != nullptr; alphabetOffset = ++i) {
		if (i >= 8) {
			i = 0;
			fin.read((char*)&byte, 1);
			checkFileDecoding();
			buffer.clear();
			buffer.append(byte);
		}
		if (buffer[i] == DOWN) {
			currentNode->left = new HuffmanNode(currentNode);
			currentNode = currentNode->left;
		}
		else {
			leavesNumber++;
			do {
				currentNode = currentNode->parent;
			} while (currentNode != nullptr && currentNode->right != nullptr);
			if (currentNode != nullptr) {
				currentNode->right = new HuffmanNode(currentNode);
				currentNode = currentNode->right;
			}
		}
	}

	for (auto i = 0U; i < leavesNumber; i++) {
		fin.read((char*)&byte, 1);
		checkFileDecoding();
		buffer.append(byte);
	}
	auto alphabet = buffer.subBitString(alphabetOffset, 8 * leavesNumber).getBytes();
	buffer.erase(0, alphabetOffset + 8 * leavesNumber);
	assignLeaves(root, alphabet);

	if (buffer.size() < LAST_BITS_INFO_SIZE) {
		fin.read((char*)&byte, 1);
		checkFileDecoding();
		buffer.append(byte);
	}
	unsigned char lastUnreadableBitsAmount = 0;
	for (auto i = LAST_BITS_INFO_SIZE - 1, j = 1; i >= 0; i--, j *= 2) {
		if (buffer[i]) {
			lastUnreadableBitsAmount += j;
		}
	}
	buffer.erase(0, LAST_BITS_INFO_SIZE);

	currentNode = root;
	if (buffer.size() == 0) {
		fin.read((char*)&byte, 1);
		checkFileDecoding();
		buffer.append(byte);
	}
	for (auto currentBit = 0U; (fin.tellg() == FILE_SIZE) ?
		currentBit < buffer.size() - lastUnreadableBitsAmount :
		currentBit < buffer.size();
		currentBit++) {
		if (buffer[currentBit] == LEFT) {
			if (currentNode->left) {
				currentNode = currentNode->left;
			}
			else if (root->left != nullptr && root->right != nullptr) {
				throw new HuffmanException("File is corrupted");
			}
		}
		else {
			if (currentNode->right) {
				currentNode = currentNode->right;
			}
			else {
				throw new HuffmanException("File is corrupted");
			}
		}
		if (currentNode->left == nullptr && currentNode->right == nullptr) {
			fout << currentNode->byte;
			currentNode = root;
		}
		if (currentBit == buffer.size() - 1) {
			fin.read((char*)&byte, 1);
			if (fin.good()) {
				buffer.clear();
				buffer.append(byte);
				currentBit = -1;	//because of increment at next iteration. that, it's unsigned, doesn't matter
			}
		}
	}

	fin.close();
	fout.close();
	root->deleteSubtree();
}

void Huffman::assignLeaves(HuffmanNode* node, std::string& alphabet) {
	if (node->left == nullptr && node->right == nullptr) {
		node->byte = alphabet[0];
		alphabet.erase(alphabet.begin());
		return;
	}
	if (node->left) {
		assignLeaves(node->left, alphabet);
	}
	if (node->right) {
		assignLeaves(node->right, alphabet);
	}
}
#include "Huffman.h"
#include "PriorityQueue.h"
#include <iostream>

#define DOWN  1
#define UP    0
#define LEFT  1
#define RIGHT 0

using namespace std;


void Huffman::setSourceFile(string sourceFile) {
	if (fin.is_open()) {
		fin.close();
	}
	fin.open(sourceFile, ios::binary | ios::in);
	if (!fin.is_open()) {
		throw HuffmanException("cannot open source file");
	}
}

void Huffman::setDestinationFile(string destinationFile) {
	if (fout.is_open()) {
		fout.close();
	}
	fout.open(destinationFile, ios::binary | ios::out);
	if (!fout.is_open()) {
		throw HuffmanException("cannot open destination file");
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
		BitString left = currentCode;////////????
		left.addBit(LEFT);
		deepFirstSearch(node->left, treeStructure, alphabet, codingTable, left);
		if (node->right) {
			BitString right = currentCode;////////????
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
	checkFile();
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
	for (int i = 0; i < BYTE_RANGE; i++) {
		if (freqs[i] > 0) {
			queue.push(HuffmanNode((unsigned char)i, freqs[i]));
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
	HuffmanNode *root = new HuffmanNode();
	*root = queue.poll();
	//we've built Huffman's tree 

	BitString treeStructure;
	BitString codingTable[BYTE_RANGE];
	string alphabet;
	deepFirstSearch(root, treeStructure, alphabet, codingTable);
	if (root->left == nullptr && right == nullptr) {
		codingTable[root->byte].addBit(LEFT); //assume the root as left son
	}
	
	BitString buffer;
	buffer.append(treeStructure);
	buffer.append(alphabet);
	fin.clear();
	fin.seekg(0, fin.beg);
	while (!fin.eof()) {
		unsigned char byte;
		fin.read((char*)&byte, 1);
		if (!fin.eof()) {
			if (buffer.getBitsCount() % 8 == 0) {
				fout << buffer.getBytes();
				buffer.clear();
			}
			buffer.append(codingTable[byte]);
		}
	}
	if (buffer.getBitsCount() > 0) {
		fout << buffer.getBytes();
	}

	root->deleteSubtree();
	fin.close();
	fout.close();
}

void Huffman::decode() {
	checkFile();
	//const short BYTE_RANGE = 256;
	BitString buffer;
	HuffmanNode* root = new HuffmanNode();
	unsigned char byte;
	fin.read((char*)&byte, 1);
	buffer.append(byte);
	size_t alphabetOffset = 1;
	size_t leavesNumber   = 0;
	HuffmanNode* current  = root;
	for (int i = 0; current != nullptr; alphabetOffset = ++i) {
		if (i >= 8) {
			i = 0;
			fin.read((char*)&byte, 1);
			buffer.clear();
			buffer.append(byte);
		}
		if (buffer[i] == DOWN) {
			current->left = new HuffmanNode(current);
			current = current->left;
		}
		else {
			leavesNumber++;
			do {
				current = current->parent;
			} while (current != nullptr && current->right);
			if (current != nullptr) {
				current->right = new HuffmanNode(current);
				current = current->right;
			}
		}
	}

	for (int i = 0; i < leavesNumber; i++) {
		fin.read((char*)&byte, 1);
		buffer.append(byte);
	}
	BitString::Iterator iterator = BitString::Iterator(buffer);
	iterator.move(alphabetOffset);
	string alphabet = iterator.getBytes(leavesNumber);
	
	root->deleteSubtree();
}
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
	if (!fin.is_open()) {
		throw new HuffmanException("cannot open source file");
	}
}

void Huffman::setDestinationFile(string destinationFile) {
	if (fout.is_open()) {
		fout.close();
	}
	fout.open(destinationFile, ios::binary | ios::out);
	if (!fout.is_open()) {
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
		BitString left = currentCode;
		left.addBit(LEFT);
		deepFirstSearch(node->left, treeStructure, alphabet, codingTable, left);
		if (node->right) {
			BitString right = currentCode;
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
	const size_t FILE_SIZE = fin.tellg();
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
	for (auto i = 0; i < BYTE_RANGE; i++) {
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
	if (root->left == nullptr && root->right == nullptr) {
		codingTable[root->byte].addBit(LEFT); //assume the root as left son
	}

	size_t compressedSize = 0;	//in bits
	for (auto i = 0; i < BYTE_RANGE; i++) {
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

	root->deleteSubtree();
	fin.close();
	fout.close();
}

void Huffman::decode() {
	checkFileOpening();

	fin.seekg(0, fin.end);
	const size_t FILE_SIZE = fin.tellg();
	fin.seekg(0, fin.beg);

	BitString buffer;
	//size_t currentByte = 0;
	HuffmanNode* root = new HuffmanNode();
	unsigned char byte;

	//переделать в одну функцию(макрос)
	fin.read((char*)&byte, 1);	
	//++currentByte;
	checkFileDecoding();
	buffer.append(byte);
	///////////////////////////////////

	size_t alphabetOffset;
	size_t leavesNumber = 0;
	HuffmanNode* current  = root;
	for (auto i = 0; current != nullptr; alphabetOffset = ++i) {
		if (i >= 8) {
			i = 0;
			fin.read((char*)&byte, 1);
			//++currentByte;
			checkFileDecoding();
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
			} while (current != nullptr && current->right != nullptr);
			if (current != nullptr) {
				current->right = new HuffmanNode(current);
				current = current->right;
			}
		}
	}

	for (auto i = 0; i < leavesNumber; i++) {
		fin.read((char*)&byte, 1);
		//++currentByte;
		checkFileDecoding();
		buffer.append(byte);
	}
	string alphabet = buffer.subBitString(alphabetOffset, 8 * leavesNumber).getBytes();
	assignLeaves(root, alphabet);
	buffer.erase(0, alphabetOffset + 8 * leavesNumber);

	if (buffer.size() < LAST_BITS_INFO_SIZE) {
		fin.read((char*)&byte, 1);
		//++currentByte;
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




	while (!fin.eof()) {
		bool isDecoded = false;
		HuffmanNode* currentNode = root;	//продумать для одноэлементного дерева
		size_t currentBit;
		for (currentBit = 0; 
			currentBit < (((size_t)fin.tellg() < FILE_SIZE - 1)? buffer.size() : buffer.size() - lastUnreadableBitsAmount);
			currentBit++) {
			//if (root->left == nullptr && root->right == nullptr) {	//for tree with one leaf (at root)

			//}
			if (buffer[currentBit] == LEFT && currentNode->left) {
				currentNode = currentNode->left;
			}
			else if (buffer[currentBit] == RIGHT && currentNode->right) {
				currentNode = currentNode->right;
			}
			if (currentNode->left == nullptr && currentNode->right == nullptr) {
				buffer.erase(0, currentBit + 1);
				fout << currentNode->byte;
				isDecoded = true;
				break;
			}
				//очистить буфер. записать в файл расшифрованный символ. 
		}
		if (!fin.eof() && !isDecoded) {
			fin.read((char*)&byte, 1);
			if (fin.good()) {
				//++currentByte;
				buffer.append(byte);
			}
		}
		//сосчитать следующий байт
	}
	
	root->deleteSubtree();
}

void Huffman::assignLeaves(HuffmanNode* node, std::string& alphabet) {
	if (node->left == nullptr && node->right == nullptr) {
		node->byte = alphabet[0];
		//alphabet = alphabet.substr(1, alphabet.size() - 1);
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

//char* Huffman::findLeaf(HuffmanNode* node, BitString& code) {
//
//}
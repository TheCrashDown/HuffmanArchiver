//#pragma once

#include "stream.h"
#include <map>
#include <algorithm>
#include <vector>

typedef std::map<byte, std::string> encodeTable;

//  построение дерева кодирования
void buildTree(std::vector<Node*>& heap, std::map<byte, int> count);
//  постороение таблицы ассоциаций <символ-код>
void buildCodeTable(encodeTable& codeTable, Node* root, std::string code);
//  побитовая запись символа в конец строки
void writeByte(std::string& str, byte symb);
//  побитовая запись int в конец строки
void writeInteger(std::string& str, int x);
//  запись таблицы кодов в строку
void writeTree(Node* root, std::string& tree);
//  кодирование строки и запись ее в compressed
void writeCompressedFile(std::string& compressed, std::string& original, encodeTable codeTable);
//  наполнение строки незначащими битами в конце до целого числа байтов
int fillStringToBytes(std::string& original);
//  запись строки в поток
void writeStringToStream (IOutputStream& compressed, std::string str);
//  запись закодированного файла в поток
void writeEncodedToStream(IOutputStream& compressed, int codeTableSize, int filledBits, std::string outString);
//  кодирование
void Encode(IInputStream& original, IOutputStream& compressed);

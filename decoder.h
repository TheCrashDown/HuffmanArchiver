//#pragma once

#include "stream.h"
#include <map>
#include <vector>

typedef std::map<std::string, byte> decodeTable;

//  чтение из потока 4 байтов и сборка int из них
int readIntegerFromStream(IInputStream& stream);
//  чтение из строки 8 символов и сборка байта из них
byte readByte(std::string& str, int& pos);
//  чтение твблицы кодов
void readCodeTable(std::string& input, decodeTable& codeTable, int codeTableSize, int& pos, std::string code);
//  чтение и декодирование оставшейся части файла
void readAndDecodeFile(std::string& input, IOutputStream& decoded, decodeTable& codeTable, int pos, int filledBits);
//  декодирование
void Decode(IInputStream& compressed, IOutputStream& original);
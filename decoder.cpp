#include "encoder.h"
#include "decoder.h"

int readIntegerFromStream(IInputStream& stream){
    //  функция читает из потока 4 байта и собирает из них int
    int res = 0;
    byte tmp = 0;
    std::string tmpS = "";
    for(int i = 0; i < 4; ++i)
    {
        stream.Read(tmp);
        writeByte(tmpS, tmp);
    }

    for (int i = 0; i < tmpS.size(); ++i)
    {
        res *= 2;
        res += tmpS[i] - '0';
    }

    return res;
}


byte readByte(std::string& str, int& pos){
    //  функция читает из строки 8 символов и собирает из них байт
    byte tmp = 0;
    int count = 0;
    while(count < 8){
        tmp *= 2;
        tmp += str[pos] - '0';
        pos++;
        count++;
    }
    return tmp;

}


void readCodeTable(std::string& input, decodeTable& codeTable, int codeTableSize, int& pos, std::string code = ""){
    //  функция читает из входной строки таблицу кодов и пихает их в map-таблицу codeTable
    if(pos >= codeTableSize){
        return;
    }
    if(input[pos] == '1'){
        pos++;
        readCodeTable(input, codeTable, codeTableSize, pos, code + "0");
        readCodeTable(input, codeTable, codeTableSize, pos, code + "1");
    } else {
        ++pos;
        codeTable[code] = readByte(input, pos);
    }
}


void readAndDecodeFile(std::string& input, IOutputStream& decoded, decodeTable& codeTable, int pos, int filledBits){
    /*  функция читает данные из оставшейся части строки,
        пренебрегая незначащими битами, и декодирует их,
        используя таблицу codeTable
    */
    std::string currentCode = "";
    while(pos < input.size() - filledBits){
        currentCode += input[pos];
        if(codeTable.find(currentCode) != codeTable.end()){
            decoded.Write(codeTable[currentCode]);
            currentCode = "";
        }
        pos++;
    }

}


void Decode(IInputStream& compressed, IOutputStream& original)
{
    //  читаем ведущие байты
    int codeTableSize = readIntegerFromStream(compressed);
    int filledBits = readIntegerFromStream(compressed);


    //  читаем оставшиеся байты - таблица и файл
    std::string input = "";
    byte tmp = 0;
    while(compressed.Read(tmp)){
        writeByte(input, tmp);
    }

    //  читаем таблицу с кодами
    int pos = 0;
    decodeTable codeTable;
    readCodeTable(input, codeTable, codeTableSize, pos);

    //  читаем + декодируем файл
    pos = codeTableSize;

    readAndDecodeFile(input, original, codeTable, pos, filledBits);

}
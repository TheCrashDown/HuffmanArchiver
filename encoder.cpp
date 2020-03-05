#include "encoder.h"

void buildTree(std::vector<Node*>& heap, std::map<byte, int> count){
    //  функция строит дерево кодирования, используя кучу

    for(auto it = count.begin(); it != count.end(); ++it){
        heap.push_back(new Node(it->first, it->second));
    }
    std::make_heap(heap.begin(), heap.end(), compareNode);

    //  объединяем элементы с наименьшими приоритетами, кидаем их обратно в кучу,
    //  добавляем указатели на потомков
    //  в итоге в куче остается 1 элемент - корень дерева
    while(true){
        if(heap.size() == 1){
            break;
        }
        //первый элемент (будущий левый потомок)
        pop_heap(heap.begin(), heap.end(), compareNode);
        Node* left = heap.back();
        heap.pop_back();

        //второй элемент (будущий правый потомок)
        pop_heap(heap.begin(), heap.end(), compareNode);
        Node* right = heap.back();
        heap.pop_back();

        //собираем новый элемент
        Node* tmp = new Node(0, left->count + right->count);
        tmp->left = left;
        tmp->right = right;

        //пихаем его в кучу
        heap.push_back(tmp);
        push_heap(heap.begin(), heap.end(), compareNode);
    }
}


void buildCodeTable(encodeTable& codeTable, Node* root, std::string code = ""){
    //  функция строит таблицу ассоциаций <символ-код> на осове построенного дерева
    if(root->left == nullptr && root->right == nullptr){
        codeTable[root->symb] = code;
    }
    if(root->left != 0){
        buildCodeTable(codeTable, root->left, code + "0");
    }
    if(root->right != 0){
        buildCodeTable(codeTable, root->right, code + "1");
    }
}


void writeByte(std::string& str, byte symb){
    //  функция побитово записывает данный символ в конец строки
    int pos = 0;
    std::string tmp = "";
    while(pos < 8){
        tmp = std::to_string(symb % 2) + tmp;
        symb /= 2;
        pos++;
    }
    str += tmp;
}


void writeInteger(std::string& str, int x){
    //  функция побитово записывает данной число типа int в конец строки
    int pos = 0;
    std::string tmp = "";
    while(pos < 32){
        tmp = std::to_string(x % 2) + tmp;
        x /= 2;
        pos++;
    }
    str += tmp;
}


void writeTree(Node* root, std::string& tree){
    /*  функция записывает таблицу кодов в данную строку
        структура :
            находимся не в узле - печатаем "1"
            находимся в узле    - печатаем "0", затем 8 бит - символ
        для восстановления данных достаточно идти по единицам до
        первого нуля, при встрече нуля - читать следующие 8 бит разом
    */
    if(root == nullptr){
        return;
    }
    if(root->left == nullptr){
        tree += "0";
        //tree += root->symb;
        writeByte(tree, root->symb);
    } else {
        tree += "1";
        writeTree(root->left, tree);
        writeTree(root->right, tree);
    }
}


void writeCompressedFile(std::string& compressed, std::string& original, encodeTable codeTable){
    //  функция кодирует строку и записывает в compressed
    byte tmp = 0;
    for(int i = 0; i < original.size(); ++i){
        compressed += codeTable[original[i]];
    }
}


int fillStringToBytes(std::string& original){
    /*  функция наполняет строку в конце незначащими битами до целого числа битов
        и возвращает число добавленных символов
    */
    int num = 0;
    while(original.size() % 8 != 0){
        original += "0";
        num++;
    }
    return num;
}


void writeStringToStream (IOutputStream& compressed, std::string str){
    /*  функция записывает строку в поток
            алгоритм :
                * собираем биты в байт
                * отсылаем байт в поток
                * начинаем собирать следующий байт
            pos         -   текущая позиция в строке
            posByte     -   количество собранных битов (если 8, то пора отсылать готовый байт)
    */
    int pos = 0;
    int posByte = 0;
    byte tmp = 0;

    while(pos <= str.size()){
        if(posByte < 8){
            tmp *= 2;
            tmp += (str[pos] - '0');
            pos++;
            posByte++;
        } else if (posByte > 0){
            compressed.Write(tmp);
            posByte = 0;
        }
    }
}


void writeEncodedToStream(IOutputStream& compressed, int codeTableSize, int filledBits, std::string outString){
    //  функция записи закодированного файла в поток вывода

    //  записываем ведущие байты
    std::string tmp = "";
    writeInteger(tmp, codeTableSize);
    writeInteger(tmp, filledBits);

    writeStringToStream(compressed, tmp);

    //  записываем строку с таблицей и закодированным файлом
    writeStringToStream(compressed, outString);
}



void Encode(IInputStream& original, IOutputStream& compressed)
{
    //  запоминаем входную строку
    std::string input = "";

    //  подсчет количеств вхождений символов
    std::map<byte, int> count;
    byte tmp = 0;
    while(original.Read(tmp)){
        input += tmp;
        count[tmp]++;
    }

    //  строим дерево кодов, используя кучу
    //  наименьшие приоритеты идут наверх
    std::vector<Node*> heap;
    buildTree(heap, count);

    //  строим таблицу с кодами
    encodeTable codeTable;
    buildCodeTable(codeTable, heap[0]);

    /*  структура вывода :
            4 байта             ---   размер таблицы с кодами
            4 байта             ---   количество нензачащих битов в конце строки (N)
        в строке outString :
            таблица с кодами
            закодированная строка
        остальное будет занесено в поток вручную перед записью строки
    */

    //  запись дерева в строку
    std::string outString;
    writeTree(heap[0], outString);

    //  запоминаем размер таблицы кодов в битах
    int codeTableSize = outString.size();

    //  запись последовательности
    writeCompressedFile(outString, input, codeTable);

    //  заполняем стоку до незначащими битами до целого цисла байтов
    int filledBits = fillStringToBytes(outString);

    //  записываем все в поток
    writeEncodedToStream(compressed, codeTableSize, filledBits, outString);
}







#pragma once

#include <iostream>
#include <fstream>
#include <string>

typedef unsigned char byte;

struct IInputStream {
    // Возвращает false, если поток закончился
    virtual bool Read(byte& value) = 0;
};

struct IOutputStream {
    virtual void Write(byte value) = 0;
};

struct CInputStream : public IInputStream {
    std::string buffer;
    int pos = 0;
    virtual bool Read(byte& value)
    {
        if (pos < buffer.size()) {
            value = buffer[pos++];
            return true;
        }
        else {
            return false;
        }
    }
};

struct COutputStream : public IOutputStream {
    std::string buffer;

    virtual void Write(byte value)
    {
        buffer += value;
    }
};

struct FInputStream : public IInputStream {
    std::ifstream stream;
    FInputStream(std::string path){
        stream.open(path);
    };

    virtual bool Read(char& value){
        stream.read(&value, 1);
        return !stream.eof();
    }
};

struct FOutputStream : public IOutputStream {
    std::ofstream stream;
    FOutputStream(std::string path){
        stream.open(path);
    };

    virtual void Write(char value){
        stream.write(&value, 1);
    }
};


struct Node
{
    byte symb = 0;
    int count = 0;
    Node* left = nullptr;
    Node* right = nullptr;
    Node(byte _symb, int _count) : symb(_symb), count(_count){}
};


inline bool compareNode(const Node* a, const Node* b){
    return a->count > b->count;
}
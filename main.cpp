#include "stream.h"
#include "encoder.h"
#include "decoder.h"

#include <iostream>

int main()
{
    setlocale(LC_ALL,"Russian");
    CInputStream in1;
    std::string in_str;
    std::cin >> in_str;
    //in1.buffer = "Last night. You see in this world there's two kinds of people my friend - those with loaded guns, and those who dig. You dig.";
    in1.buffer = in_str;
    COutputStream out1;
    Encode(in1, out1);

    CInputStream in2;
    in2.buffer = out1.buffer;
    COutputStream out2;
    Decode(in2, out2);

    std::cout << "\n========decoded=======\n\n";
    std::cout << out2.buffer << "\n\n";

    std::cout << "ratio: " << 1.0 * out1.buffer.size() / in1.buffer.size() << std::endl;
    std::cout << "is ok: " << (in1.buffer == out2.buffer) << std::endl;



    return 0;
}
#include <iostream>
#include "huffman/huffman.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Input File Not Detected\n";
        exit(1);
    }

    huffman f;
    f.decompress(argv[1]);

    cout << "File Decompressed successfully" << endl;

    return 0;
}
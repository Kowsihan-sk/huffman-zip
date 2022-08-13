#include <iostream>
#include "huffman/huffman.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Input File Not Detected\n";
        exit(1);
    }

    huffman f;
    f.compress(argv[1]);

    cout << "File Compressed successfully" << endl;

    return 0;
}
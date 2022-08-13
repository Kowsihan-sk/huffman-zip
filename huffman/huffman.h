// to prevent header files from being included multiple times
#ifndef HUFFMAN_H
#define HUFFMAN_H
// #include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <vector>

using namespace std;

struct TreeNode {
    char val;
    unsigned int freq;
    TreeNode* left, * right;

    TreeNode() : left(nullptr), right(nullptr) {}
    TreeNode(char x, unsigned int freq) : val(x), freq(freq), left(nullptr), right(nullptr) {}
    TreeNode(char x, unsigned int freq, TreeNode* left, TreeNode* right) : val(x), freq(freq), left(left), right(right) {}
};

class Compare {
public:
    // to make minHeap based on frequency of characters
    bool operator() (TreeNode* l, TreeNode* r) {
        return l->freq > r->freq;
    }
};

class huffman {
private:
    // making frequency table from input string
    unordered_map<char, unsigned int> buildFreqTable(string);

    // making minimum Heap based on Frequency of characters
    priority_queue<TreeNode*, vector<TreeNode*>, Compare > buildHeap(unordered_map<char, unsigned int>);

    // building tree from minHeap
    TreeNode* buildTree(priority_queue<TreeNode*, vector<TreeNode*>, Compare >);

    // utility function to traverse tree and set string value
    void treeTraverse(TreeNode*, string, unordered_map<char, string>&);
    // build corresponding string code to characters
    unordered_map<char, string> buildCodeDict(TreeNode*);

    // utility function to convert decimal int to binary string
    string decimalToBinary(int, int);

    // utility function to convert binary string to decimal int
    int binaryToDecimal(string, int);

    // build encoded and padded string from input string
    string buildEncodedText(string, unordered_map<char, string>);

    // converting encoded binary data to decimal byte (8 bits) array  
    vector<int> buildByteArray(string);

    // it will minHeap meta data which will be used during decompressing the file
    string heapMetaData(priority_queue<TreeNode*, vector<TreeNode*>, Compare >, unordered_map<char, string>);

    // build codeToChar Dictionary based on minHeap meta data from input file
    unordered_map<string, char> buildCharDict(string);

    // get EncodedText by converting decimal bytes from input file to binary data and then removing padding from the result
    string getEncodedText(string);

    // build original text string from encoded text using codeToChar Dictionary
    string buildOriginalText(string, unordered_map<string, char>);
public:
    // function to compress input text file to a bin file 
    void compress(string);

    // function to decompress bin file to a text file 
    void decompress(string);
};

#endif
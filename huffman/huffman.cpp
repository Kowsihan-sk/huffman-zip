#include "huffman.h"

unordered_map<char, unsigned int> huffman::buildFreqTable(string s) {
    unordered_map<char, unsigned int> freq;
    for (char c : s) {
        freq[c]++;
    }

    return freq;
}

priority_queue<TreeNode*, vector<TreeNode*>, Compare > huffman::buildHeap(unordered_map<char, unsigned int> freq) {
    priority_queue<TreeNode*, vector<TreeNode*>, Compare > pq;
    for (auto it : freq) {
        TreeNode* node = new TreeNode(it.first, it.second);
        pq.push(node);
    }

    return pq;
}

TreeNode* huffman::buildTree(priority_queue<TreeNode*, vector<TreeNode*>, Compare > minHeap) {
    TreeNode* root;

    while (minHeap.size() > 1) {
        TreeNode* node1 = minHeap.top();
        minHeap.pop();

        TreeNode* node2 = minHeap.top();
        minHeap.pop();

        root = new TreeNode();
        root->freq = node1->freq + node2->freq;

        root->left = node1;
        root->right = node2;

        minHeap.push(root);
    }

    return root;
}

void huffman::treeTraverse(TreeNode* node, string s, unordered_map<char, string>& charToCode) {
    if (!node) return;
    if (!node->left and !node->right) {
        charToCode[node->val] = s;
        return;
    }

    treeTraverse(node->left, s + '0', charToCode);
    treeTraverse(node->right, s + '1', charToCode);
}

unordered_map<char, string> huffman::buildCodeDict(TreeNode* root) {
    unordered_map<char, string> charToCode;

    treeTraverse(root, "", charToCode);

    return charToCode;
}

int huffman::binaryToDecimal(string s, int base = 2) {
    return stoi(s, 0, base);
}

string huffman::decimalToBinary(int n, int bits = 8) {
    string str = "";
    for (int i = bits - 1; i >= 0; i--) {
        if (1 & (n >> i))
            str += "1";
        else
            str += "0";
    }

    return str;
}

string huffman::buildEncodedText(string text, unordered_map<char, string> charToCode) {
    string str = "";
    for (char c : text) {
        str += charToCode[c];
    }

    // cout << str << "\n";

    // * add padding 
    int padding_val = 8 - (str.size() % 8);
    for (int i = 0; i < padding_val; i++)
        str += '0';

    string padding_info = decimalToBinary(padding_val);

    str = padding_info + str;

    return str;
}

vector<int> huffman::buildByteArray(string s) {
    vector<int> v;
    for (int i = 0; i < s.size(); i += 8) {
        int x = binaryToDecimal(s.substr(i, 8));
        v.push_back(x);
    }

    return v;
}

string huffman::heapMetaData(priority_queue<TreeNode*, vector<TreeNode*>, Compare > minHeap, unordered_map<char, string> charToCode) {
    string meta = "";
    meta += (char)minHeap.size();
    while (!minHeap.empty()) {
        TreeNode* node = minHeap.top();
        minHeap.pop();

        meta += node->val;
        string s(127 - charToCode[node->val].size(), '0');
        s += '1';
        s += charToCode[node->val];

        for (int i = 0; i < s.size(); i += 8) {
            meta += (char)binaryToDecimal(s.substr(i, 8));
        }
    }

    return meta;
}

unordered_map<string, char> huffman::buildCharDict(string inFileName) {
    unordered_map<string, char> codeToChar;

    fstream inFile;
    inFile.open(inFileName, ios::in | ios::binary);

    inFile.seekg(0);

    unsigned char minHeapSize;
    inFile.read(reinterpret_cast<char*>(&minHeapSize), 1);

    for (int i = 0; i < minHeapSize; i++) {
        char c;
        unsigned char code[16];
        inFile.read(&c, 1);
        inFile.read(reinterpret_cast<char*>(code), 16);

        string codeStr = "";
        for (int j : code)
            codeStr += decimalToBinary(j);

        int j = 0;
        while (codeStr[j] == '0') j++;
        codeStr = codeStr.substr(j + 1);

        codeToChar[codeStr] = c;
    }

    inFile.close();
    return codeToChar;
}

string huffman::getEncodedText(string inFileName) {
    fstream inFile;
    inFile.open(inFileName, ios::in | ios::binary);

    inFile.seekg(0);

    unsigned char minHeapSize;
    inFile.read(reinterpret_cast<char*>(&minHeapSize), 1);

    inFile.seekg(minHeapSize * 17 + 1, ios::beg);

    unsigned char cur;
    string encodedTextWithPadding = "";
    inFile.read(reinterpret_cast<char*>(&cur), 1);
    while (!inFile.eof()) {
        encodedTextWithPadding += decimalToBinary((int)cur);
        inFile.read(reinterpret_cast<char*>(&cur), 1);
    }

    int padding_val = binaryToDecimal(encodedTextWithPadding.substr(0, 8));

    string encodedText = encodedTextWithPadding.substr(8, encodedTextWithPadding.size() - 8 - padding_val);

    inFile.close();
    return encodedText;
}

string huffman::buildOriginalText(string encodedText, unordered_map<string, char> codeToChar) {
    string output = "";
    for (int i = 0; i < encodedText.size();) {
        for (int j = i; j < encodedText.size(); j++) {
            string cur = encodedText.substr(i, j - i + 1);
            if (codeToChar.count(cur)) {
                output += codeToChar[cur];
                i = j + 1;
                break;
            }
        }
    }

    return output;
}

void huffman::compress(string inFileName) {

    fstream inFile;
    inFile.open(inFileName, ios::in);

    // * reading input file to string input
    string input = "", temp;
    while (getline(inFile, temp))
        input += temp + "\n";

    inFile.close();

    // * removing extra \n
    input = input.substr(0, input.size() - 1);

    unordered_map<char, unsigned int> freq = buildFreqTable(input);

    priority_queue<TreeNode*, vector<TreeNode*>, Compare > minHeap = buildHeap(freq);

    TreeNode* root = buildTree(minHeap);

    unordered_map<char, string> charToCode = buildCodeDict(root);

    string encodedText = buildEncodedText(input, charToCode);

    vector<int> byteArray = buildByteArray(encodedText);


    // * setting output file name
    int idx = inFileName.find_last_of(".");
    string outFileName = inFileName.substr(0, idx) + "-compressed.bin";

    // * output file
    fstream outFile;
    outFile.open(outFileName, ios::out | ios::binary);

    // * huffman heap meta data
    // * we will use this information to decompress the file
    string meta = heapMetaData(minHeap, charToCode);
    // * writing minHeap meta data
    outFile << meta;

    // * writing byte array to output file
    for (int i : byteArray) {
        outFile << (char)i;
    }

    outFile.close();
}

void huffman::decompress(string inFileName) {
    unordered_map<string, char> codeToChar = buildCharDict(inFileName);

    string encodedText = getEncodedText(inFileName);

    string outputText = buildOriginalText(encodedText, codeToChar);


    // * setting output file name
    int idx = inFileName.find_last_of("-");
    if (idx == -1) idx = inFileName.find_last_of(".");
    string outFileName = inFileName.substr(0, idx) + "-decompressed.txt";

    // * output file
    fstream outFile;
    outFile.open(outFileName, ios::out);

    outFile << outputText;

    outFile.close();
}
#include <vector>
#include <queue>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "HCNode.hpp"
#include "HCTree.hpp"

using namespace std;

int main(int /*argc*/, char *argv[])
{
    ifstream inFile(argv[1], ifstream::in|ifstream::binary);
    ofstream outFile(argv[2], ofstream::out|ofstream::binary);

    // Check if inFile is empty
    inFile.seekg(0, ios::end);
    streampos end = inFile.tellg();
    inFile.seekg(0, ios::beg);
    streampos beg = inFile.tellg();
    if (beg == end)
    {
        cout << "The input file was empty. " << endl;
        inFile.close();
        outFile.close();
        return 0;
    }

    int buffer;
    // Number of unique characters in the input file
    int uniqueElements = 0;
    if (inFile.good() && outFile.good())
    {
        // Set up the frequency vector by gathering the number of times that
        // each character appears in the file and storing it in the proper index
        vector<int> frequency;
        frequency = vector<int>(256, 0);
        while (!inFile.eof())
        {
            buffer = inFile.get();
            if (buffer != -1)
                ++frequency[buffer];
        }

        // Find the total number of unique characters in inFile
        for (int zod : frequency)
        {
            if (zod != 0)
                uniqueElements++;
        }
        outFile << uniqueElements;

        // Declare and build the Huffman Tree
        HCTree *encodeTree = new HCTree();
        encodeTree->build(frequency);

        // Output the frequency vector to the file so that it can be recovered
        // when we attempt to uncompress. Format is
        // (# of unique chars)(char)(freq of char)(char)(freq of char)
        // EX: 4 a 2 b 4 C 1 d 6

        for (unsigned int zod = 0; zod < frequency.size(); zod++)
        {
            if (frequency[zod] != 0)
            {
                outFile.put(static_cast<char>(zod));
                outFile.put(' ');
                outFile << frequency[zod];
                outFile.put(' ');
            }
        }
        outFile << endl;

        // Clear inFile so that it can be manipulated again
        inFile.clear();
        inFile.seekg(0);
        // Compress the input file by calling HCTree's encode function
        while (!inFile.eof())
        {
            if (inFile.peek() == -1) break;
            encodeTree->encode((byte)inFile.get(), outFile);
        }
        // Close files and destroy the tree
        inFile.close();
        outFile.close();
        delete encodeTree;
    }
    else
        cout << "The input file does not exist. " << endl;
    return 0;
}

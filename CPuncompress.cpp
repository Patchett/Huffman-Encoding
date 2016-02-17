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

    // Various buffers for holding input from files
    int buffer;
    string strBuffer;
    char charBuffer;
    // The number of unique characters that will be present in the uncompressed
    // file
    int uniqueElements;
    if (inFile.good() && outFile.good())
    {
        vector<int> frequency;
        frequency = vector<int>(256, 0);

        // Get the number of unique characters in the file to be decoded
        while (1)
        {
            buffer = inFile.get();
            if (buffer >= 48 && buffer <= 57)
            {
                // Append the char read from file onto strBuffer
                strBuffer.append(1, (char)buffer);
            }
            else break;
        }
        // Convert the appended string to an int
        uniqueElements = atoi(strBuffer.c_str());
        // Move the stream back because the above while loop reads one char
        // too far
        inFile.seekg(-1, ios::cur);

        // Read in the frequency vector
        for (int zod = 0; zod < uniqueElements; zod++)
        {
            strBuffer.clear();
            // Get the ASCII value of the char
            buffer = inFile.get();
            // Skip the space
            inFile.seekg(1, ios::cur);
            do
            {
                charBuffer = static_cast<char>(inFile.get());
                // If the char we just read was a number
                if (charBuffer != ' ')
                {
                    // Append the number to strBuffer
                    strBuffer.append(1, charBuffer);
                }
                else break;
            }
            while (1);

            // Convert strBuffer into an int and store it in the correct
            // place in the frequency vector.
            frequency[buffer] = atoi(strBuffer.c_str());
        }
        // Advance past newline char that will be right after the end of the
        // frequency vector in inFile
        inFile.seekg(1, ios::cur);
        // Declare and build the Huffman Tree
        HCTree *decodeTree = new HCTree();
        decodeTree->build(frequency);

        // Decompress the input file by calling HCTree's decode function
        while (!inFile.eof())
        {
            if (inFile.peek() == -1) break;
            outFile.put((char)(decodeTree->decode(inFile)));
        }
        // Close files and destroy the tree
        outFile.close();
        inFile.close();
        delete decodeTree;
    }
    else
        cout << "One of the files was not found. " << endl;
    return 0;
}

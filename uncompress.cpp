#include "BitOutputStream.hpp"
#include "BitInputStream.hpp"
#include "HCNode.hpp"
#include "HCTree.hpp"
#include <vector>
#include <queue>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc == 3)
    {
        ifstream inFile(argv[1], ifstream::in|ifstream::binary);
        ofstream outFile(argv[2], ofstream::out|ofstream::binary);
        BitInputStream inStream(inFile);

        // Check if the input file is empty
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

        if (inFile.good() && outFile.good())
        {
            // Read in the total number of chars in that the
            // uncompressed file will have
            int numOfChars = 0;
            numOfChars = inStream.readInt();

            // Declare and build the Huffman Tree by delegating to readNodes
            // to read build the tree from the input file
            HCTree *decodeTree = new HCTree();
            decodeTree->setRoot(decodeTree->readNodes(inStream));

            // Decompress the input file by calling HCTree's decode function
            int buffer;
            for (int zod = 0; zod < numOfChars; zod++)
            {
                buffer = decodeTree->decode(inStream);
                if (buffer == -1) // EOF check
                    break;
                else
                    outFile.put((unsigned char)buffer);
            }
            // Close files and destroy the tree
            outFile.close();
            inFile.close();
            delete decodeTree;
        }
    }
    else
        cout << "One of the files was not found. " << endl;
    return 0;
}

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

int main(int argc, char **argv)
{

    if (argc == 3)
    {
        ifstream inFile(argv[1], ifstream::in|ifstream::binary);
        ofstream outFile(argv[2], ofstream::out|ofstream::binary);
        BitOutputStream outStream(outFile);

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

        if (inFile.good() && outFile.good())
        {
            int buffer;
            // Total number of chars in the file to be compressed
            int numOfChars = 0;
            // Set up the frequency vector by gathering the number of times that
            // each character appears in the file and storing it in the proper index
            vector<unsigned int> frequency;
            frequency = vector<unsigned int>(256, 0);
            while (!inFile.eof())
            {
                buffer = inFile.get();
                if (!inFile.eof())
                    ++frequency[buffer];
            }

            // Find the total number of chars in the file to be compressed
            // and store it at the beginning of the compressed file
            for (int zod : frequency)
            {
                numOfChars += zod;
            }
            outStream.writeInt(numOfChars);

            // Declare and build the Huffman Tree
            HCTree *encodeTree = new HCTree();
            encodeTree->build(frequency);

            // Write the tree to the beginning of the compressed file
            encodeTree->writeNodes(encodeTree->getRoot(), outStream);

            // Clear inFile so that it can be manipulated again
            inFile.clear();
            inFile.seekg(0);

            // Compress the input file by calling HCTree's encode function
            for (int zod = 0; zod < numOfChars; zod++)
            {
                if (inFile.peek() == -1) break;
                encodeTree->encode((char)inFile.get(), outStream);
            }

            outStream.flush();
            // Close files and destroy the tree
            inFile.close();
            outFile.close();
            delete encodeTree;
        }
    }
    else
        cout << "One of the files was not found. " << endl;
    return 0;
}

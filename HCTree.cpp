#include "HCTree.hpp"
#include <vector>
#include <queue>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

void HCTree::build(const vector<unsigned int> &freqs)
{
    // Declare the priority queue that we will use to implement the
    // Huffman Tree
    std::priority_queue<HCNode *, std::vector<HCNode *>, HCNodePtrComp> huffTree;
    // A few HCNode pointers for use while building
    HCNode *first, *second, *newTree;
    // Initialize the leaves vector with HCNode pointers to nodes created from
    // the values in the freqs vector
    // Also create the forest of trees for the priority queue
    for (int zod = 0; zod < 256; zod++) {
        if (freqs[zod] != 0) {
            HCNode *newNode = new HCNode(freqs[zod], zod);
            leaves[zod] = newNode;
            huffTree.push(newNode);
        }
    }

    // Merge the forest of trees into the Huffman Tree
    while (huffTree.size() > 1) {
        // Pop 2 elements off of the queue and store them
        first = huffTree.top();
        huffTree.pop();
        second = huffTree.top();
        huffTree.pop();
        // Create an intermediate node that will be the parent of the two popped
        // nodes
        newTree = new HCNode(((first->count) + (second->count)),
                             second->symbol);
        // Set up the pointers for the new node and push it back onto the queue
        newTree->c1 = first;
        newTree->c0 = second;
        first->p = newTree;
        second->p = newTree;
        //debug
        /*cout << "newTree symbol: " << newTree->symbol << endl;
        cout << "left symbol: " << newTree->c0->symbol << endl;
        cout << "right symbol: " << newTree->c1->symbol << endl;*/
        huffTree.push(newTree);
    }
    // Set the root equal to the remaining node in the priority queue which now
    // resembles our Huffman Tree
    root = huffTree.top();
}

void HCTree::setRoot(HCNode *newRoot)
{
    root = newRoot;
}

void HCTree::writeNodes(HCNode *current, BitOutputStream &out)
{
    // current is not a leaf
    if (current->c0 != 0 && current->c1 != 0) {
        // write a 0 and recursively call this method to perform a post order
        // traversal on the Huffman Tree
        out.writeBit(0);
        writeNodes(current->c0, out);
        writeNodes(current->c1, out);
    } else {
        // current is a leaf, write a 1 and the leaf's symbol
        out.writeBit(1);
        out.writeByteBuffer(current->symbol);
    }
}

HCNode *HCTree::readNodes(BitInputStream &in)
{
    // Read in a 1 means that we have reached a leaf node and the next byte
    // holds the leaf node's symbol
    if (in.readBit() == 1) {
        // Create a new node and store the symbol
        HCNode *newNode = new HCNode(0, in.readByteBuffer());
        return newNode;
    } else {
        // Recursively reconstruct the tree
        HCNode *leftChild = readNodes(in);
        HCNode *rightChild = readNodes(in);
        HCNode *returnNode = new HCNode(0, 0, leftChild, rightChild);
        return returnNode;
    }
}

HCNode *HCTree::getRoot()
{
    return root;
}

HCTree::~HCTree()
{
    // Don't do anything if the tree does not exist
    if (root == 0) {
        std::cout << "The tree is already empty." << std::endl;
        return;
    }
    // Case if there is only a root node
    else if (root->c0 == 0 && root->c1 == 0) {
        delete root;
        return;
    } else {
        // Delegate to a recursive helper function to destroy the tree
        clearHelper(root);
        root = 0;
    }

}

void HCTree::encode(byte symbol, ofstream &out) const
{
    if (root->c0 == 0 && root->c1 == 0) {
        out.put('1');
        return;
    }
    encodeHelper(leaves[symbol], out);
}

void HCTree::encodeHelper(HCNode *current, ofstream &out) const
{
    if (current->p == 0) return;
    encodeHelper(current->p, out);
    if (current->p->c0 == current)
        out.put('0');
    else
        out.put('1');
}

void HCTree::encode(byte symbol, BitOutputStream &out) const
{
    if (root->c0 == 0 && root->c1 == 0) {
        out.writeBit(1);
        return;
    }
    encodeHelper(leaves[symbol], out);
}

void HCTree::encodeHelper(HCNode *current, BitOutputStream &out) const
{
    if (current->p == 0) return;
    encodeHelper(current->p, out);
    if (current->p->c0 == current) {
        out.writeBit(0);
    } else {
        out.writeBit(1);
    }
}

void HCTree::clearHelper(HCNode *current)
{
    /*if (current->c0 != 0)
        clearHelper(current->c0);
    if (current->c1 != 0)
        clearHelper(current->c1);
    delete current;
    return;*/
    if (current == 0)
        return;
    clearHelper(current->c0);
    clearHelper(current->c1);
    delete current;
}

int HCTree::decode(ifstream &in) const
{
    HCNode *current = root;
    // Take the next char from the file
    char charBuffer = static_cast<char>(in.get());
    // Traverse down the tree to the correct leaf based on the input
    // from the file
    while (current->c0 != 0 || current->c1 != 0) {
        if (charBuffer == '0') { // go left
            if (current->c0->c0 != 0 || current->c0->c1 != 0) {
                current = current->c0;
                charBuffer = static_cast<char>(in.get());
            } else { // The next node will be a leaf, return it
                current = current->c0;
                break;
            }
        } else { // go right
            if (current->c1->c0 != 0 || current->c1->c1 != 0) {
                current = current->c1;
                charBuffer = static_cast<char>(in.get());
            } else { // The next node will be a leaf, return it
                current = current->c1;
                break;
            }
        }
    }
    return (int)current->symbol;
}

int HCTree::decode(BitInputStream &in) const
{
    HCNode *current = root;
    int buffer = in.readBit(); // EOF check
    if (buffer == -1)
        return -1;

    while (current->c0 != 0 || current->c1 != 0) {
        if (buffer == 0) { // go left
            if (current->c0->c0 != 0 || current->c0->c1 != 0) {
                current = current->c0;
                buffer = in.readBit();
            } else {
                // The next node is a leaf, return it
                current = current->c0;
                break;
            }
        } else { // go right
            if (current->c1->c0 != 0 || current->c1->c1 != 0) {
                current = current->c1;
                buffer = in.readBit();
            } else {
                // The next node is a leaf, return it
                current = current->c1;
                break;
            }
        }
    }
    return (int)current->symbol;
}

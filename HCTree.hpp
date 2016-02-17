#ifndef HCTREE_HPP
#define HCTREE_HPP

#include <vector>
#include <queue>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "HCNode.hpp"
#include "BitInputStream.hpp"
#include "BitOutputStream.hpp"

using namespace std;

/** A 'function class' for use as the Compare class in a
 *  priority_queue<HCNode*>.
 *  For this to work, operator< must be defined to
 *  do the right thing on HCNodes.
 */
class HCNodePtrComp
{
public:
    bool operator()(HCNode *&lhs, HCNode *&rhs) const
    {
        return *lhs < *rhs;
    }
};

/** A Huffman Code Tree class.
 *  Not very generic:  Use only if alphabet consists
 *  of unsigned chars.
 */
class HCTree
{
private:
    HCNode *root;
    vector<HCNode *> leaves;

public:
    explicit HCTree() : root(0)
    {
        leaves = vector<HCNode *>(256, (HCNode *) 0);
    }

    ~HCTree();

    /**
     * Recursively encodes the tree
     * @method encodeHelper
     * @param  current      The current node in the traversal
     * @param  out          The BitOutputStream to write to
     */
    void encodeHelper(HCNode *current, BitOutputStream &out) const;

    /**
     * Uses a post order traversal to write the tree to the header of the
     * compressed file. Writes a 0 for children, writes a 1 followed by the
     * symbol for leaves
     * @method writeNodes
     * @param  current    current node in the traversal
     * @param  out        BitOutputStream to write to
     */
    void writeNodes(HCNode *current, BitOutputStream &out);

    /**
     * Reads and reconstructs the tree from the compressed file recursively
     * @method readNodes
     * @param  in        The BitInputStream to read the tree from
     * @return           Returns a pointer to the node which is the root of the
     *                   tree
     */
    HCNode *readNodes(BitInputStream &in);

    /**
     * Getter function for the root member of HCTree so that we can have direct
     * access to it in compress/uncompress
     * @method getRoot
     * @return the root of the HCTree
     */
    HCNode *getRoot();

    /**
     * Sets a new root for the HCTree
     * @method setRoot
     * @param  newRoot this will be the new root for the tree
     */
    void setRoot(HCNode *newRoot);

    /**
     * Recursively destroys every node in the Huffman Tree
     * @method clearHelper
     * @param  current     The current node in the traversal
     */
    void clearHelper(HCNode *current);

    /**
     * Recursively moves from a leaf up to the root of the tree, printing
     * 1's and 0's on the way
     * @method encodeHelper
     * @param  current      current node in the traversal
     * @param  out          stream to print 1's and 0's to
     */
    void encodeHelper(HCNode *current, ofstream &out) const;

    /** Use the Huffman algorithm to build a Huffman coding trie.
     *  PRECONDITION: freqs is a vector of ints, such that freqs[i] is
     *  the frequency of occurrence of byte i in the message.
     *  POSTCONDITION:  root points to the root of the trie,
     *  and leaves[i] points to the leaf node containing byte i.
     */
    void build(const vector<unsigned int> &freqs);

    /** Write to the given BitOutputStream
     *  the sequence of bits coding the given symbol.
     *  PRECONDITION: build() has been called, to create the coding
     *  tree, and initialize root pointer and leaves vector.
     */
    void encode(byte symbol, BitOutputStream &out) const;

    /** Write to the given ofstream
     *  the sequence of bits (as ASCII) coding the given symbol.
     *  PRECONDITION: build() has been called, to create the coding
     *  tree, and initialize root pointer and leaves vector.
     *  THIS METHOD IS USEFUL FOR THE CHECKPOINT BUT SHOULD NOT
     *  BE USED IN THE FINAL SUBMISSION.
     */
    void encode(byte symbol, ofstream &out) const;


    /** Return symbol coded in the next sequence of bits from the stream.
     *  PRECONDITION: build() has been called, to create the coding
     *  tree, and initialize root pointer and leaves vector.
     */
    int decode(BitInputStream &in) const;

    /** Return the symbol coded in the next sequence of bits (represented as
     *  ASCII text) from the ifstream.
     *  PRECONDITION: build() has been called, to create the coding
     *  tree, and initialize root pointer and leaves vector.
     *  THIS METHOD IS USEFUL FOR THE CHECKPOINT BUT SHOULD NOT BE USED
     *  IN THE FINAL SUBMISSION.
     */
    int decode(ifstream &in) const;

};

#endif // HCTREE_HPP

#include "BitOutputStream.hpp"
#include <vector>
#include <queue>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <bitset>

using namespace std;

void BitOutputStream::writeBit(int bit)
{
    // Flush buf if it is full
    if (bufi == 8)
        flush();
    ++bufi;
    buf <<= 1;
    // Put a 1 into the last bit of buf if needed
    buf = buf | bit;
}

void BitOutputStream::flush()
{
    if (bufi != 0)
    {
        // leftshift buf my 8-bufi to account for padding of 0's
        buf <<= (8-bufi);
        // write buf to file
        out.put(buf);
        out.flush();
        buf = bufi = 0;
    }
}

void BitOutputStream::writeByteBuffer(unsigned char b)
{
    // Store that bit which is extracted from input parameter
    int bitBuffer = 0;
    // Iterates 8 times to extract all 8 bits of unsigned char
    for (int zod = 0; zod < 8; zod++)
    {
        // Flush buf if it is full
        if (bufi == 8)
            flush();

        ++bufi;
        // Extract the MSB from the parameter b and leftshift b
        bitBuffer = b & 0x80;
        b <<= 1;
        // Insert the extracted bit into the LSB of buf
        if (bitBuffer == 0x80)
        {
            buf <<= 1;
            buf = buf | 0x01;
        }
        else
        {
            buf <<= 1;
        }
    }
}

void BitOutputStream::writeByte(int b)
{
    out.put((unsigned char)b);
}

void BitOutputStream::writeInt(int i)
{
    out.write((char *) &i, sizeof(i));
}



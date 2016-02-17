#include "BitInputStream.hpp"
#include <vector>
#include <queue>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <bitset>

using namespace std;

int BitInputStream::readBit()
{
    // Check if buf is empty and read a new set of 8 bits if it is
    if (bufi == 8)
    {
        // make sure we don't read EOF
        if (in.peek() == -1)
            return -1;
        else
        {
            // Read in next 8 bits and reset buf and bufi
            buf = 0;
            bufi = 0;
            buf = in.get();
        }
    }

    // Extract the MSB from buf, store it in returnMe, and leftshift buf
    unsigned char returnMe;
    returnMe = buf & 0x80;
    buf <<= 1;
    bufi++;
    return returnMe;
}

int BitInputStream::readByte()
{
    if (in.peek() == -1) // EOF check
        return -1;
    else // Read in a byte of data
        return (unsigned char)in.get();
}

unsigned char BitInputStream::readByteBuffer()
{
    // Stores bit which is extracted from buf
    int bitExtract = 0;
    // Stores cumulative bits extracted from buf
    unsigned char returnMe = 0;
    // Iterates 8 times to extract 8 bits, the size of 1 byte
    for (int zod = 0; zod < 8; zod++)
    {
        // Check if buf is empty
        if (bufi == 8)
        {
            if (in.peek() == -1) // EOF check
                return -1;
            else
            {
                // Refill buf
                buf = 0;
                bufi = 0;
                buf = in.get();
            }
        }
        // Extract MSB from buf, shift buf left, and increment bufi
        bitExtract = buf & 0x80;
        buf <<= 1;
        bufi++;

        // If extracted bit was 1, store it in LSB of returnMe
        if (bitExtract == 0x80)
        {
            returnMe <<= 1;
            returnMe = returnMe | 0x01;
        }
        // If extracted bit was 0, store it in LSB of returnMe
        else
        {
            returnMe <<= 1;
        }
    }
    return returnMe;
}

int BitInputStream::readInt()
{
    if (in.peek() == -1) // EOF check
        return -1;
    int returnMe = 0;
    // Delegate to read to read size int
    in.read((char *) &returnMe, sizeof(returnMe));
    return returnMe;
}

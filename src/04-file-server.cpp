#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
using namespace std;

#define CYLINDERS 3
#define SECTORS 8
#define BLOCK_SIZE 128

string clean() {
    ofstream freeBlocks;
    freeBlocks.open("storage/free-blocks.dsk");
    if ( freeBlocks.fail() ) {
        return "[0] ERR: Could not access \"storage/free-blocks.dsk\"\n\n";
    }
    for ( int c = 0; c < CYLINDERS; c++ ) {
        for ( int s = 0; s < SECTORS; s++ ) {
            freeBlocks << to_string(c) << ',' << to_string(s) << '\n';
        }
    }
    freeBlocks.clear();
    freeBlocks.close();
    return "[1] Successfully clean free-blocks.dsk\n\n";
}

string getAvailBlock() {
    ifstream freeBlocks;
    freeBlocks.open("storage/free-blocks.dsk");
    if ( freeBlocks.fail() ) {
        return "[0] ERR: Could not access \"storage/free-blocks.dsk\"\n\n";
    }
    string avail, line;
    getline(freeBlocks, avail);

    ofstream temp;
    temp.open("storage/.temp.free-blocks.dsk");
    if ( temp.fail() ) {
        return "[0] ERR: Could not access \"storage/.temp.free-blocks.dsk\"\n\n";
    }

    while( getline(freeBlocks, line) )
        temp << line + '\n';

    freeBlocks.clear();
    freeBlocks.close();
    temp.clear();
    temp.close();

    remove("storage/free-blocks.dsk");
    rename("storage/.temp.free-blocks.dsk", "storage/free-blocks.dsk");

    return avail;
}

int main() {
    cout << clean();
    cout << getAvailBlock();
}
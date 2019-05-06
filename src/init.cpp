#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
using namespace std;

#define CYLINDERS 3
#define SECTORS 8
#define BLOCK_SIZE 128

string resetFreeBlocks() {
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
    return "[1] Successfully cleaned free-blocks.dsk\n\n";
}

string resetHDD() {
    ofstream disk;
    disk.open("storage/hdd.dsk");
    if ( disk.fail() ) {
        return "\n\n*** ERROR: Could not access file \"storage/hdd.dsk\". ***\n\n";
    }
    
    string zeroFillBlock = "";
    for ( int i = 0; i < BLOCK_SIZE; i++ )
        zeroFillBlock += '0';
    zeroFillBlock += '\n';

    for ( int i = 0; i < CYLINDERS; i++ ) {
        for ( int j = 0; j < SECTORS; j++ ) {
            disk << zeroFillBlock;
        }
    }
    disk.clear();
    disk.close();
    return "[1] Successfully cleaned hdd.dsk.\n\n";
}

string resetTree() {
    ofstream tree;
    tree.open("storage/tree.dsk");
    if ( tree.fail() ) {
        return "[0] ERR: Could not access \"storage/tree.dsk\"\n\n";
    }
    tree << "root/,0,-1,-1\n";
    tree.clear();
    tree.close();
    return "[1] Successfully cleaned tree.dsk\n\n";
}

string resetFAT() {
    ofstream fat;
    fat.open("storage/fat.dsk");
    if ( fat.fail() ) {
        return "[0] ERR: Could not access \"storage/fat.dsk\"\n\n";
    }
    fat.clear();
    fat.close();
    return "[1] Successfully cleaned fat.dsk\n\n";
}

int main() {
    cout << resetFreeBlocks();
    cout << resetHDD();
    cout << resetTree();
    cout << resetFAT();
    cout << "Set Up Complete.\n\n";
    return 0;
}
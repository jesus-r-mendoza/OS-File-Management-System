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

const string indicator = "<~/#+?$=&>";

vector<string> split(string str, string regex) {
    int found;
    vector<string> vect;
    string segment;
    while ( ( found = str.find(regex) ) != -1 ) {
        segment = str.substr(0, found);
        str.erase(0, found + regex.length());
        if ( segment != " " && segment.length() > 0 )
            vect.push_back(segment);
    }
    if ( str.length() > 0 )
        vect.push_back(str);
    return vect;
}

int blockIndex(int c, int s) {
    return ( c * SECTORS ) + s;
}

string reset() {
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

string createFile(int fileID, string fileName) {
    string avail = getAvailBlock();
    if ( avail.length() == 0 )
        return "[0] ERR: No more free space available.\n\n";
    vector<string> coor = split(avail, ",");
    string writeData = "";
    for ( int i = 0; i < BLOCK_SIZE - indicator.length() - 3; i++ )
        writeData += '0';
    writeData += indicator + "eof";

    // send() request to disk server
    cout << writeData << endl;
    cout << writeData.length() << endl;
    // receive confirmation from disk server that data was written

    ifstream fat;
    fat.open("storage/fat.dsk");
    if ( fat.fail() ) {
        return "[0] ERR: Could not access \"storage/fat.dsk\"\n\n";
    }
    ofstream temp;
    temp.open("storage/.temp.fat.dsk");
    if ( temp.fail() ) {
        return "[0] ERR: Could not access \"storage/.temp.fat.dsk\"\n\n";
    }
    string line;
    while ( getline(fat, line) )
        temp << line + '\n';
    int block = blockIndex(stoi(coor[0]), stoi(coor[1]));
    temp << fileID << ',' << fileName << ",1," << to_string(block) << ',' << avail << '\n';
    fat.clear();
    fat.close();
    temp.clear();
    temp.close();
    remove("storage/fat.dsk");
    rename("storage/.temp.fat.dsk", "storage/fat.dsk");
}

int main() {
    cout << reset();
    createFile(12, "a.txt");
}
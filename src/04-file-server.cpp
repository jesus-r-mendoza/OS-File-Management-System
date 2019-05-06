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

string addAvailBlock(string avail) {
    ofstream freeBlocks;
    freeBlocks.open("storage/free-blocks.dsk", ios::app);
    if ( freeBlocks.fail() ) {
        return "[0] ERR: Could not access \"storage/free-blocks.dsk\"\n\n";
    }
    freeBlocks << avail + '\n';
    freeBlocks.clear();
    freeBlocks.close();
    return "[1] Successfully added free block.\n\n";
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

    ofstream fat;
    fat.open("storage/fat.dsk", ios::app);
    if ( fat.fail() ) {
        return "[0] ERR: Could not access \"storage/fat.dsk\"\n\n";
    }
    int block = blockIndex(stoi(coor[0]), stoi(coor[1]));
    fat << fileID << ',' << fileName << ",1," << to_string(block) << ',' << avail << '\n';
    fat.clear();
    fat.close();
    return "[1] Successfully created a new file.\n\n";
}

string deleteFile(int fileID) {
    string id = to_string(fileID);
    ifstream fat;
    fat.open("storage/fat.dsk");
    if ( fat.fail() ) {
        return "[0] ERR: Could not access \"storage/fat.dsk\"\n\n";
    }
    string line;

    fstream temp;
    temp.open("storage/.temp.fat.dsk");
    if ( temp.fail() ) {
        return "[0] ERR: Could not access \"storage/.temp.fat.dsk\"\n\n";
    }
    vector<string> parts;
    string cyl = "-1";
    string sec = "-1";
    while ( getline(fat, line) ) {
        parts = split(line, ",");
        if ( id == parts[0] ) {
            cyl = parts[4];
            sec = parts[5];
        } else
            temp << line + '\n';
    }
    if ( cyl == "-1" && sec == "-1" ) {
        fat.clear();
        fat.close();
        temp.clear();
        temp.close();
        return "[0] ERR: This file doesn't exist. Cannot be deleted.\n\n";
    } 
    addAvailBlock(cyl + ',' + sec);
    // send "R" request to disk server with cyl and sec that was gathered
    // receive confirm notice that block exits
    // capture string that holds the file's data
    string data = "";
    parts = split(data, indicator);
    vector<string> nums;
    while ( parts[1] != "eof" ) {
        nums = split(parts[1], ",");
        cyl = nums[0];
        sec = nums[1];
        // send "R" request to disk server with cyl and sec that was gathered
        // receive confirm notice that block exits
        // capture string that holds the file's data
        data = ""; // whatever was received from read request
        addAvailBlock(cyl + ',' + sec);
        parts = split(data, indicator);
    }
    return "[1] Successfully delete the file. And allocated all of its blocks as free.\n\n";
}

string list(string flag) {
    if ( flag != "0" && flag != "1" )
        return "[0] ERR: Unrecognized flag.\n\n";
    ifstream fat;
    fat.open("storage/fat.dsk");
    if ( fat.fail() ) {
        return "[0] ERR: Could not access \"storage/fat.dsk\"\n\n";
    }
    string line;
    string result = (flag == "0" ? "All files:\n" : "id\t{Name}\t(Blocks)\t<Index>\t[cyl]\t[sec]\n");
    vector<string> parts;
    while ( getline(fat, line) ) {
        parts = split(line, ",");
        if ( flag == "0" ) 
            result += parts[1] + '\n';
        else
            result += parts[0] + "\t{" + parts[1] + "}\t(" + parts[2] + ")\t<" + parts[3] + ">\t[" + parts[4] + "]\t[" + parts[5] + "]\n";
    }
    result += '\n';
    return result;
}

int main() {
    cout << reset();
    createFile(12, "a.txt");
    cout << list("1");
}
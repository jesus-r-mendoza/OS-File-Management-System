#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <fstream>
using namespace std;

#define CYLINDERS 3
#define SECTORS 8
#define BLOCK_SIZE 128

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

bool inBounds(int c, int s) {
    return c >= 0 && c < CYLINDERS && s >= 0 && s < SECTORS;
}

string info(vector<string> args) {
    if ( args.size() != 1 )
        return "ERR: Command \"I\" requires 0 arguments\n\n";
    else {
        return "  Disk Format \n     Cylinders: " + to_string(CYLINDERS) + " \n       Sectors: " + to_string(SECTORS) + "\n\n";
    }
}

string read(vector<string> args) {
    if ( args.size() != 3 ) {
        return "[0] ERR: Command \"R\" requires 2 arguments\n\n";
    }
    ifstream disk;
    disk.open("src/hdd.dsk");
    if ( disk.fail() ) {
        return "[0] ERR: Could not access \"hdd.dsk\"\n\n";
    }
    int cyl = -1;
    int sec = -1;
    try {
        cyl = stoi(args[1]);
        sec = stoi(args[2]);
    } catch (invalid_argument) {
        return "[0] ERR: Use integer values\n\n";
    }
    if ( !inBounds(cyl, sec) ) {
        return "[0] ERR: Paramters not in bound. Use \"I\" cmd to see disk format.\n\n";
    }
    int index = blockIndex(cyl, sec);
    string line;
    for ( int i = 0; i <= index; i++ )
        getline(disk, line);
    
    disk.clear();
    disk.close();
    return "[1] " + line + "\n\n";
}

string write(vector<string> args) {
    if ( args.size() != 5 ) {
        return "[0] ERR: Command \"W\" requires 4 arguments\n\n";
    }
    int cyl = -1;
    int sec = -1;
    int len = -1;
    try {
        cyl = stoi(args[1]);
        sec = stoi(args[2]);
        len = stoi(args[3]);
    } catch (invalid_argument) {
        return "[0] ERR: Use integer values\n\n";
    }
    if ( len != args[4].length() ) {
        return "[0] ERR: Data is different length. Expected " + to_string(len) + " bytes," +
               " Received " + to_string(args[4].length()) + " bytes.\n\n";
    }
    if ( !inBounds(cyl, sec) ) {
        return "[0] ERR: Paramters not in bound. Use \"I\" command to see disk format.\n\n";
    }
    ifstream disk;
    disk.open("src/hdd.dsk");
    if ( disk.fail() ) {
        cout << "0 - Could not access \"hdd.dsk\"\n\n";
        return 0;
    }
    ofstream temp;
    temp.open("src/.temp.dsk");
    if ( temp.fail() ) {
        return "[0] ERR: Could not access \".temp.dsk\"\n\n";
    }
    int index = blockIndex(cyl, sec);
    string line;
    string data = "";
    data += args[4];
    for ( int i = 0; i < BLOCK_SIZE - len; i++ )
        data += '0';
    for ( int i = 0; i < CYLINDERS * SECTORS; i++ ) {
        getline(disk, line);
        if ( i == index )
            temp << data;
        else
            temp << line;
        temp << "\n";
    }
    
    disk.clear();
    disk.close();
    temp.clear();
    temp.close();
    remove("src/hdd.dsk");
    rename("src/.temp.dsk", "src/hdd.dsk");
    return "[1] Data successfully writen.\n\n";
}

string help() {
    return "You asked for help\n\n";
}

string processArgs(vector<string> args) {
    if ( args.size() == 0 )
        return NULL;
    if ( args[0] == "I" )
        return info(args);
    else if ( args[0] == "R" )
        return read(args);
    else if ( args[0] == "W" )
        return write(args);
    else if ( args[0] == "help" )
        return help();
    else
        return "ERR: Command \"" + args[0] + "\" not found. Use command \"help\" for more info.\n\n";
}

bool prompt() {
    string input;
    cout << "usr@root/ $ ";
    getline(cin, input);
    vector<string> arguments = split(input, " ");
    if (arguments.size() == 0) return true;
    cout << processArgs(arguments);
    return false;
}

int main() {
    bool quit = false;
    while ( !quit )
        quit = prompt();
}
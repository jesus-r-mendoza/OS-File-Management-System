#include <iostream>
#include <string>
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

void info(vector<string> args) {
    if ( args.size() != 1 )
        cout << "ERR: Command \"I\" requires 0 arguments\n\n";
    else {
        cout << "+--------------+" << endl; 
        cout << "| Disk Format: |" << endl;
        cout << "|  " << CYLINDERS << " Cylinders |" << endl;
        cout << "|  " << SECTORS << " Sectors   |" << endl;
        cout << "+--------------+" << endl;
    }
}

int read(vector<string> args) {
    if ( args.size() != 3 ) {
        cout << "ERR: Command \"R\" requires 2 arguments\n\n";
        return 0;
    }
    ifstream disk;
    disk.open("src/hdd.dsk");
    if ( disk.fail() ) {
        cout << "0 - Could not access \"hdd.dsk\"\n\n";
        return 0;
    }
    int cyl = -1;
    int sec = -1;
    try {
        cyl = stoi(args[1]);
        sec = stoi(args[2]);
    } catch (invalid_argument) {
        cout << "0 - Use integer values\n\n";
        return 0;
    }
    if ( !inBounds(cyl, sec) ) {
        cout << "0 - Paramters not in bound. Use \"I\" cmd to see disk format.\n\n";
        return 0;
    }
    int index = blockIndex(cyl, sec);
    string line;
    for ( int i = 0; i <= index; i++ )
        getline(disk, line);
    cout << "1 " << line << "\n\n";

    disk.clear();
    disk.close();
    return 1;
}

int write(vector<string> args) {
    if ( args.size() != 5 ) {
        cout << "ERR: Command \"W\" requires 4 arguments\n\n";
        return 0;
    }
    int cyl = -1;
    int sec = -1;
    int len = -1;
    try {
        cyl = stoi(args[1]);
        sec = stoi(args[2]);
        len = stoi(args[3]);
    } catch (invalid_argument) {
        cout << "0 - Use integer values\n\n";
        return 0;
    }
    if ( len != args[4].length() ) {
        cout << "0 - Data is different length. Expected " << len << " bytes,";
        cout << " Received " << args[4].length() << " bytes.\n\n";
        return 0;
    }
    if ( !inBounds(cyl, sec) ) {
        cout << "0 - Paramters not in bound. Use \"I\" cmd to see disk format.\n\n";
        return 0;
    }
    ifstream disk;
    disk.open("src/hdd.dsk");
    if ( disk.fail() ) {
        cout << "0 - Could not access \"hdd.dsk\"\n\n";
        return 0;
    }
    ofstream temp;
    temp.open("src/.temp.dsk");
    if ( disk.fail() ) {
        cout << "0 - Could not access \".temp.dsk\"\n\n";
        return 0;
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
    cout << "1 " << "\n\n";
    disk.clear();
    disk.close();
    temp.clear();
    temp.close();
    remove("src/hdd.dsk");
    rename("src/.temp.dsk", "src/hdd.dsk");
    return 1;
}

void help() {
    cout << "You asked for help\n\n";
}

bool processArgs(vector<string> args) {
    if ( args.size() == 0 )
        return false;
    if ( args[0] == "I" )
        info(args);
    else if ( args[0] == "R" )
        read(args);
    else if ( args[0] == "W" )
        write(args);
    else if ( args[0] == "help" )
        help();
    else if ( args[0] == "exit" ) {
        cout << "\nExiting...\nBye\n\n";
        return true;
    } else
        cout << "ERR: Command \"" << args[0] << "\" not found. Use command \"help\" for more info.\n\n";
    return false;
}

bool prompt() {
    string input;
    cout << "usr@root/ $ ";
    getline(cin, input);
    vector<string> arguments = split(input, " ");
    bool quit = processArgs(arguments);
    return quit;
}

int main(int argc, char* args[]) {
    bool quit = false;
    while ( !quit )
        quit = prompt();
}
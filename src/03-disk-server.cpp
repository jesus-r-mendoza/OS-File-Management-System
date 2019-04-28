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

void read(vector<string> args) {
    if ( args.size() != 3 ) 
        cout << "ERR: Command \"R\" requires 2 arguments\n\n";
    else {
        ifstream disk;
        disk.open("src/hdd.dsk");
        if ( disk.fail() ) {
            cout << "0 - Could not access \"hdd.dsk\"\n\n";
        }
        try {
            int cyl = stoi(args[1]);
            int sec = stoi(args[2]);
            if ( !inBounds(cyl, sec) )
                cout << "0 - Paramters not in bound. Use \"I\" cmd to see disk format.\n\n";
            else {
                int index = blockIndex(cyl, sec);
                string line;
                for ( int i = 0; i <= index; i++ )
                    getline(disk, line);
                cout << "grabbed line: " << line << "\n\n";
            }
        } catch (invalid_argument) {
            cout << "0 - Use integer values\n\n";
        }
        disk.clear();
        disk.close();
    }
}

void write(vector<string> args) {
    if ( args.size() != 5 ) 
        cout << "ERR: Command \"W\" requires 4 arguments\n\n";
    else
        cout << "Executing: W " << args[1] << " " << args[2] << " " << args[3] << " " << args[4] << "\n\n";
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
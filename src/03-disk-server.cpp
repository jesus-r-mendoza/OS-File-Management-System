#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <vector>
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

bool processArgs(vector<string> args) {
    if ( args[0] == "I" )
        cout << "+--------------+\n| Disk Format: | \n|  3 Cylinders | \n|  8 Sectors   | \n+--------------+\n\n";
    else if ( args[0] == "R" ) {
        if ( args.size() != 3 ) 
            cout << "ERR: Command \"R\" requires 2 arguments\n\n";
        else
            cout << "Executing: R " << args[1] << " " << args[2] << "\n\n";
    } else if ( args[0] == "W" ) {
        if ( args.size() != 4 ) 
            cout << "ERR: Command \"W\" requires 3 arguments\n\n";
        else
            cout << "Executing: W " << args[1] << " " << args[2] << " " << args[3] << "\n\n";
    } else if ( args[0] == "exit" ) {
        cout << "\nExiting...\nBye\n\n";
        return true;
    } else 
        cout << "ERR: Command \"" << args[0] << "\" not found. Use command \"help\" for more info.\n\n";
    return false;
}

bool prompt() {
    string input;
    cout << "\nusr@root/ $ ";
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
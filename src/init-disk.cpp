#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
using namespace std;

#define CYLINDERS 3
#define SECTORS 8
#define BLOCK_SIZE 128

int main() {
    
    ofstream disk;
    disk.open("hdd.dsk");
    if ( disk.fail() ) {
        cerr << "\n\n*** ERROR: Could not access file \"hdd.dsk\". ***\n\n";
        return 1;
    }

    cout << "Initializing hard drive disk file.";
    
    string zeroFillBlock = "";
    for ( int i = 0; i < BLOCK_SIZE; i++ )
        zeroFillBlock += '0';
    zeroFillBlock += '\n';

    for ( int i = 0; i < CYLINDERS; i++ ) {
        for ( int j = 0; j < SECTORS; j++ ) {
            disk << zeroFillBlock;
        }
        cout << '.';
    }
    disk.clear();
    disk.close();
    cout << "\nDone.\n";
    return 0;
}
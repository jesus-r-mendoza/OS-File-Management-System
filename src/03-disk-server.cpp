#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <fstream>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
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
        return "NULL";
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

int getConnection() {
    cout << "\n[ Finding new connection... ]\n";
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if ( sock < 0 ) {
    	cout << "ERR: socket() failed.\n";
    	return -1;
    }
	
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(49000);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
 
    int b = bind(sock, (sockaddr*)&hint, sizeof(hint));
    if ( b < 0 ) {
    	cout << "ERR: bind() failed.\n";
    	return -1;
    }
    
    int l = listen(sock, SOMAXCONN);
    if ( l < 0 ) {
    	cout << "ERR: listen() failed.\n";
    	return -1;
    }
    
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
 
    int clientSocket = accept(sock, (sockaddr*)&client, &clientSize);
    
    if ( clientSocket < 0 ) {
    	cout << "ERR: Couldn't connect with client\n";
    	return -1;
    }
    
    close(sock);
    
    return clientSocket;
}

int main() {
    char buf[256];
    int client = -1;
    
    do {
    	if ( client < 0 ) {
            client = getConnection();
            if ( client < 0 ) continue;
            else cout << "\n[ Connected to client. ]\n\n";
    	}
		
        int bytesReceived = recv(client, buf, 256, 0);
        
        if ( bytesReceived < 0 ) {
            cout << "ERR: recv() failed.\n";
            continue;
        } else if ( bytesReceived == 0 ) {
            cout << "\n[ Client disconnected. ]\n";
            close(client);
            client = -1;
            continue;
        }
		
        string input = string(buf, 0, bytesReceived);
        cout << "Client Requested: " << input << endl;
        vector<string> arguments = split(input, " ");
        string result = processArgs(arguments);
        
        int s = send(client, result.c_str(), result.length(), 0);
        if ( s < 0 )
            cout << "ERR: send() failed.\n";
        else
            cout << "Server Response Successful.\n";
        
    } while (true);
    
    close(client);    
    return 0;
}
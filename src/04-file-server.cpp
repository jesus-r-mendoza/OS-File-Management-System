#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
using namespace std;

#define CYLINDERS 3
#define SECTORS 8
#define BLOCK_SIZE 128
#define PORT 49000
const string ip = "127.0.0.1";
const string indicator = "<~/#+?$=&>";
char buf[256];

int diskServerSocket = -1;
int fileServerSocket = -1;

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

int connectToDiskServer() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if ( sock < 0 ) {
    	cout << "ERR: socket() failed.\n";
    	return -1;
    }

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);
    inet_pton(AF_INET, ip.c_str(), &hint.sin_addr);
	
    int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if ( connectRes < 0 ) {
        cout << "Couldn't connect to server.\n";
        return -1;
    }
	
    cout << "\n[ Connected to server. ]\n\n";
    return sock;
}

string sendAndRecv(string msg) {
    int sendRes = send(diskServerSocket, msg.c_str(), msg.size() + 1, 0);
    if ( sendRes < 0 ) {
        return "[0] ERR: send() failed.\n";
    }
    int bytesReceived = recv(diskServerSocket, buf, 256, 0);
    if ( bytesReceived < 0 ) {
        return "[0] ERR: recv() failed.\n";
    } else if ( bytesReceived == 0 ) {
		return "[0] ERR: [ Server disconnected. ]\n";
	} else {
        string response = string(buf, bytesReceived);
        if ( response != "NULL" ) return response;
    }
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
    hint.sin_port = htons(51000);
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

int blockIndex(int c, int s) {
    return ( c * SECTORS ) + s;
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

    string cmdRequest = "W " + coor[0] + " " + coor[1] + " " + to_string(128) + " " + writeData;
    string response = sendAndRecv(cmdRequest);
    cout << "server responded: " << response << endl;

    if ( response[1] != '1' )
        return "[0] ERR: The server rejected the request.\n\n";

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

    ofstream temp;
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
        remove("storage/.temp.fat.dsk");
        return "[0] ERR: This file doesn't exist. Cannot be deleted.\n\n";
    }
    string cmdRequest = "R " + cyl + " " + sec;
    string response = sendAndRecv(cmdRequest);
    cout << "server resonded: " << response << endl;

    if ( response[1] != '1' )
        return "[0] ERR: The server rejected the request.\n\n";

    string data = response.erase(0, 4).substr(0,128);
    cout << data << endl;
    addAvailBlock(cyl + ',' + sec);

    parts = split(data, indicator);
    cout << parts.size() << " ; eof ? " << parts[1] << "  of size: " << parts[1].length() << endl; 
    vector<string> nums;
    while ( parts[1] != "eof" ) {
        nums = split(parts[1], ",");
        cyl = nums[0];
        sec = nums[1];
        cmdRequest = "R " + cyl + " " + sec;
        response = sendAndRecv(cmdRequest);
        cout << "server resonded: " << response << endl;

        if ( response[1] != '1' )
            return "[0] ERR: The server rejected the request.\n\n";

        string data = response.erase(0, 4);
        addAvailBlock(cyl + ',' + sec);
        parts = split(data, indicator);
    }
    fat.clear();
    fat.close();
    temp.clear();
    temp.close();
    remove("storage/fat.dsk");
    rename("storage/.temp.fat.dsk", "storage/fat.dsk");
    return "[1] Successfully delete the file. And allocated all of its blocks as free.\n\n";
}

string list(string flag) {
    if ( flag != "0" && flag != "1" )
        return "[0] ERR: Unrecognized flag \"" + flag + "\".\n\n";
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

string processArgs(vector<string> args) {
    if ( args.size() == 0 )
        return "NULL";
    if ( args[0] == "C" )
        return createFile(stoi(args[1]),args[2]);
    else if ( args[0] == "D" )
        return deleteFile(stoi(args[1]));
    else if ( args[0] == "L" )
        return list(args[1]);
    else
        return "ERR: Command \"" + args[0] + "\" not found. Use command \"help\" for more info.\n\n";
}

int main() {
    
    diskServerSocket = connectToDiskServer();
    if ( diskServerSocket < 0 ) {
        cout << "Failed to connect to server.\n\n";
        return -1;
    }
    int fails = 0;
    do {
        if ( fails > 15 ) {
            cout << "\nToo many connection fails. Terminating.\n\n";
            break;
        }

        if ( fileServerSocket < 0 ) {
            fileServerSocket = getConnection();
            if ( fileServerSocket < 0 ) {
                fails++;
                continue;
            }
            else cout << "\n[ Connected to client. ]\n\n";
    	}
		
        int bytesReceived = recv(fileServerSocket, buf, 256, 0);
        
        if ( bytesReceived < 0 ) {
            cout << "ERR: recv() failed.\n";
            continue;
        } else if ( bytesReceived == 0 ) {
            cout << "\n[ Client disconnected. ]\n";
            close(fileServerSocket);
            fileServerSocket = -1;
            continue;
        }
		
        string input = string(buf, 0, bytesReceived);
        cout << "Client Requested: " << input << endl;
        vector<string> arguments = split(input, " ");
        string result = processArgs(arguments);
        
        int s = send(fileServerSocket, result.c_str(), result.length(), 0);
        if ( s < 0 )
            cout << "ERR: send() failed.\n";
        else
            cout << "Server Response Successful.\n";

    } while(true);

    close(diskServerSocket);
    return 0;
}
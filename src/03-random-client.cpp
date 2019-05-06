#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
using namespace std;

#define CYLINDERS 20
#define SECTORS 20
#define BLOCK_SIZE 128

char buf[256];
char chars[] = {'!','#','$','%','&','(',')','*','+',',','-','.','/','0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?','@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','[',']','^','_','`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};

int diskServerSocket = -1;

int connectToServer() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if ( sock < 0 ) {
    	cout << "ERR: socket() failed.\n";
    	return -1;
    }
	
    int port = 49000;
    string ipAddress = "127.0.0.1";

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);
	
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

void randReadsAndWrites(int n) {
    int cmd, cyl, sec, letr;
    string data = "";
    string request, response;
    cout << "Starting random R / W requests.";
    for ( int i = 0; i < n; i++ ) {
        cmd = rand() % 2;
        if ( cmd == 0 ) {
            cyl = rand() % CYLINDERS;
            sec = rand() % SECTORS;
            request = "R " + to_string(cyl) + " " + to_string(sec);
            response = sendAndRecv(request);
            if ( response[1] == '1' ) cout << ".";
        } else {
            cyl = rand() % CYLINDERS;
            sec = rand() % SECTORS;
            for ( int j = 0; j < BLOCK_SIZE; j++ ) {
                letr = rand() % 87;
                data += chars[letr];
            }
            request = "W " + to_string(cyl) + " " + to_string(sec) + " " + to_string(BLOCK_SIZE) + " " + data;
            response = sendAndRecv(request);
            if ( response[1] == '1' ) cout << ".";
            data = "";
        }
    }
    cout << "done.\n\n";
}

int main(int argc, char* args[]) {
    int N = 0;
    int seed = -1;    
    if ( argc != 2 && argc != 3 ) {
        cout << "\n   Usage: $ ./random-client <N>\n";
        cout <<   "   Usage: $ ./random-client <N> <RAND SEED>\n\n";
        return 1;
    }
    try {
        N = stoi(args[1]);
        if ( argc == 3 ) srand(seed);
        else srand(time(0));
    } catch ( invalid_argument ) {
        cout << "\n   Usage: $ ./random-client <N>\n";
        cout <<   "   Usage: $ ./random-client <N> <RAND SEED>\n\n";
        return 1;
    }

    diskServerSocket = connectToServer();
    if ( diskServerSocket < 0 )
        return -1;
    
    randReadsAndWrites(N);

    close(diskServerSocket);
    cout << "[ Closed connection ]\n\n";
}
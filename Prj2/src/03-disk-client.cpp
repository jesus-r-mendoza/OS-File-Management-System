#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
using namespace std;

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if ( sock < 0 ) {
    	cout << "ERR: socket() failed.\n";
    	return 1;
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
        return 1;
    }
	
    cout << "\n[ Connected to server. ]\n\n";

    char buf[256];
    string input;
    
    do {
        cout << "usr@root/ $ ";
        getline(cin, input);
        
        if ( input == "exit" ) {
            cout << "\nExiting...\nBye\n\n";
            break;
        }
        int sendRes = send(sock, input.c_str(), input.size() + 1, 0);
        if ( sendRes < 0 ) {
            cout << "ERR: send() failed.\n";
            continue;
        }
		
        int bytesReceived = recv(sock, buf, 256, 0);
        if ( bytesReceived < 0 ) {
            cout << "ERR: recv() failed.\n";
        } else if ( bytesReceived == 0 ) {
			cout << "\n[ Server disconnected. ]\n\n";
			return 1;
		} else {
            string response = string(buf, bytesReceived);
            if ( response != "NULL" ) cout << response;
        }
        
    } while (true);
	
    close(sock);
    return 0;
}

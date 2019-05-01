#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
using namespace std;

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
		
        cout << string(buf, 0, bytesReceived) << endl;
        
        int s = send(client, buf, bytesReceived, 0);
        if ( s < 0 )
        	cout << "ERR: send() failed.\n";
        
    } while (true);
    
    close(client);    
    return 0;
}

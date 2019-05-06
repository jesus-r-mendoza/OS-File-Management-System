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

int count = 0;
int fileServerSocket = -1;
char buf[256];

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

int connectToFileServer() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if ( sock < 0 ) {
    	cout << "ERR: socket() failed.\n";
    	return -1;
    }
    string ip = "127.0.0.1";
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(51000);
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
    int sendRes = send(fileServerSocket, msg.c_str(), msg.size() + 1, 0);
    if ( sendRes < 0 ) {
        return "[0] ERR: send() failed.\n";
    }
    int bytesReceived = recv(fileServerSocket, buf, 256, 0);
    if ( bytesReceived < 0 ) {
        return "[0] ERR: recv() failed.\n";
    } else if ( bytesReceived == 0 ) {
		return "[0] ERR: [ Server disconnected. ]\n";
	} else {
        string response = string(buf, bytesReceived);
        if ( response != "NULL" ) return response;
    }
}

class Node {
    public:
    Node(string nombre) {
        name = nombre;
        isDir = isDirectory();
        id = count++;
    }
    int id = -1;
    Node* parent = NULL;
    string name;
    vector<Node*> children;
    bool isDir;
    int cyl = -1;
    int sec = -1;
    
    int addChild(Node* child) {
        if ( child == NULL )
            return -2;
        for ( Node* x : children ) {
            if ( x->name == child->name )
                return -1;
        }
        child->parent = this;
        children.push_back(child);
        return 0;
    }

    int delChild(Node* child) {
        if ( child == NULL )
            return -2;
        int i = 0;
        int d = -1;
        for ( Node* x : children ) {
            if ( x->name == child->name )
                d = i;
            i++;
        }
        if ( !(d < 0) ) {
            children.erase(children.begin()+d);
            return 0;
        }
        return -1;
    }

    Node* getChild(string child) {
        if ( child.length() <= 0 )
            return NULL;
        for ( Node* x : children ) {
            if ( x->name == child ) 
                return x;
        }
        return NULL;
    }

    bool isDirectory() {
        return name[name.length()-1] == '/';
    }

    Node* dfsHelper(string node) {
        if ( name == node ) 
            return this;
        for ( Node* c : children ) {
            Node* temp = c->dfsHelper(node);
            if ( temp != NULL )
                return temp;
        }
        return NULL;
    }

    Node* dfsHelperID(int node) {
        if ( id == node ) 
            return this;
        for ( Node* c : children ) {
            Node* temp = c->dfsHelperID(node);
            if ( temp != NULL )
                return temp;
        }
        return NULL;
    }

    string saveSubtree() {
        string content = "";
        // par id, name, id, cyl, sec
        content += to_string(parent->id) + ',' + name + ',' + to_string(id) + ','  + to_string(cyl) + ',' + to_string(sec) + '\n';
        for ( Node* child : children )
            content += child->saveSubtree(); 
        return content;       
    }
};

class Tree {
    public:
    Tree(Node* raiz) {
        root = raiz;
        current = root;
    }
    Node* root;
    Node* current = root;

    string ls() {
        string list = "";
	    if(current->isDir && current->children.size() > 0) {
            list = "   ";
            for ( Node* child : current->children )
                list += (child->name) + "   \t";
            list += '\n';
        }
        list += '\n';
        return list;
    }
    
    string pwd() {
        Node* curr = current;
        string path = "" + curr->name;
        while ( curr != NULL && curr->parent != NULL ) {
            curr = curr->parent;
            path = curr->name + path;
        }
        return path;
    }

    string cd(string child) {
        if ( child == ".." ) {
            if ( current->parent != NULL )
                current = current->parent;
            else
                return "[0] ERR: Cannot move to a higher directory.\nUsage: $ cd dirname/\n\n";
        } else if ( child == "~" || child == "root/" || child == "root" ) {
            current = root; 
        } else {
            Node* c = current->getChild(child);
            if ( c != NULL && c->isDir )
                current = c;
            if ( c == NULL ) {
                vector<string> longPath = split(child, "/");
                if ( longPath.size() > 1 ) {
                    c = current;
                    c = c->getChild(longPath[0]+"/");
                    for ( int i = 1; i < longPath.size(); i++ ) {
                        if ( c == NULL ) break;
                        c = c->getChild(longPath[i]+"/");
                    }
                    if ( c != NULL && c->isDir )
                        current = c;                    
                }
            }
            if ( c != NULL && !c->isDir )
                return "[0] ERR: \"" + c->name + "\" is not a valid directory.\nUsage: $ cd dirname/\n";
            if ( c == NULL )
                return "[0] ERR: No such directoy \"" + child + "\" exists.\nUsage: $ cd dirname/\n\n";
        }
        return "[1] Successfully changed directory.\n\n";
    }

    string mkdir(string subdir) {
        if ( subdir.length() <= 0 || subdir[subdir.length()-1] != '/' )
            return "[0] ERR: \"" + subdir + "\" is not a valid directory name.\nUsage: $ mkdir dirname/\n\n";
        vector<string> longPath = split(subdir, "/");
        int result;
        if ( longPath.size() == 1 ) {
            Node* dir = new Node(longPath[0]+'/');
            result = current->addChild(dir);
            if ( result == -2 )
                return "[0] ERR: Couldn't create directory.\nUsage: $ mkdir dirname/\n\n";
            else if ( result == -1 )
                return "[0] ERR: Directory already exits.\nUsage: $ mkdir dirname/\n\n";
            return "[1] Successfully created directory.\n\n";
        }
        string dir = "";
        for ( int i = 0; i < longPath.size()-1; i++ )
            dir += longPath[i] + '/';
        string res = cd(dir);
        if ( res[1] == '1' )
            return mkdir(longPath[longPath.size()-1] + '/');
        else
            return "[0] ERR: Couldn't create directory \"" + subdir + "\".\nUsage: $ mkdir dirname/";
    }

    string rmdir(string subdir) {
        if ( subdir.length() <= 0 || subdir[subdir.length()-1] != '/' )
            return "[0] ERR: \"" + subdir + "\" is not a valid directory name.\nUsage: $ rmdir dirname/\n\n";
        vector<string> longPath = split(subdir, "/");
        int result;
        if ( longPath.size() == 1 ) {
            Node dir(longPath[0]+'/');
            result = current->delChild(&dir);
            if ( result == -2 )
                return "[0] ERR: Couldn't delete directory.\nUsage: $ rmdir dirname/\n\n";
            else if ( result == -1 )
                return "[0] ERR: Directory \"" + subdir + "\" doesn't exits.\nUsage: $ rmdir dirname/\n\n";
            return "[1] Successfully deleted directory.\n\n";
        }
        string dir = "";
        for ( int i = 0; i < longPath.size()-1; i++ )
            dir += longPath[i] + '/';
        string res = cd(dir);
        cout << "res: " << res << "dir: " << dir << endl;
        if ( res[1] == '1' )
            return rmdir(longPath[longPath.size()-1] + '/');
        else
            return "[0] ERR: Couldn't delete directory \"" + subdir + "\".\nUsage: $ rmdir dirname/";
    }

    string touch(string fileName) {
        if ( fileName.length() <= 0 || fileName[fileName.length()-1] == '/' )
            return "[0] ERR: \"" + fileName + "\" is not a valid file name.\nUsage: $ touch filename\n\n";
        vector<string> longPath = split(fileName, "/");
        int result;
        if ( longPath.size() == 1 ) {
            Node* dir = new Node(longPath[0]);
            result = current->addChild(dir);
            if ( result == -2 )
                return "[0] ERR: Couldn't create file.\nUsage: $ touch filename\n\n";
            else if ( result == -1 )
                return "[0] ERR: File already exits.\nUsage: $ touch filename\n\n";
            string cmdRequest = "C " + to_string(dir->id) + " " + longPath[0];
            string response = sendAndRecv(cmdRequest);
            if ( response[1] != '1' )
                return response;
            return "[1] Successfully created file.\n\n";
        }
        string dir = "";
        for ( int i = 0; i < longPath.size()-1; i++ )
            dir += longPath[i] + '/';
        string res = cd(dir);
        if ( res[1] == '1' )
            return touch(longPath[longPath.size()-1]);
        else
            return "[0] ERR: Couldn't create file \"" + fileName + "\".\nUsage: $ touch filename";
    }

    string allFilesArgs(vector<string> args) {
        if ( args.size() != 2 )
            return "[0] ERR: Command \"L\" requires 1 arguments.\n\n";
        return sendAndRecv("L " + args[1]);
    }

    Node* dfs(string node) {
        if ( root->name == node )
            return root;
        for ( Node* child : root->children ) {
            if ( child->name == node ) 
                return child;
            Node* temp = child->dfsHelper(node);
            if ( temp != NULL )
                return temp;
        }
        return NULL;
    }

    Node* dfsID(int node) {
        if ( root->id == node )
            return root;
        for ( Node* child : root->children ) {
            if ( child->id == node ) 
                return child;
            Node* temp = child->dfsHelperID(node);
            if ( temp != NULL )
                return temp;
        }
        return NULL;
    }
    
    string lsArgs(vector<string> args) {
        if ( args.size() != 1 )
            return "[0] ERR: Command\"ls\" requires 0 arguments.\n\n";
        return ls();
    }

    string pwdArgs(vector<string> args) {
        if ( args.size() != 1 )
            return "[0] ERR: Command \"pwd\" requires 0 arguments.";
        return pwd();
    }

    string cdArgs(vector<string> args) {
        if ( args.size() != 2 )
            return "[0] ERR: Command \"cd\" requires 1 argument.\nUsage: $ cd dirname/\n\n";
        return cd(args[1]);
    }

    string mkdirArgs(vector<string> args) {
        if ( args.size() != 2 )
            return "[0] ERR: Command \"mkdir\" requires 1 argument.\nUsage: $ mkdir dirname/\n\n";
        int a = args[1].find(",");
        if ( a != -1 )
            return "[0] ERR: \"" + args[1] + "\" is not a valid directory name.\nUsage: $ mkdir dirname/\n\n";
        return mkdir(args[1]);
    }

    string rmdirArgs(vector<string> args) {
        if ( args.size() != 2 )
            return "[0] ERR: Command \"rmdir\" requires 1 argument.\nUsage: $ rmdir dirname/\n\n";
        return rmdir(args[1]);
    }

    string touchArgs(vector<string> args) {        
        if ( args.size() != 2 )
            return "[0] ERR: Command \"touch\" requires 1 argument.\nUsage: $ touch filename\nUsage: $ touch filename\n\n";
        int a = args[1].find(",");
        int b = args[1].find("/");
        if ( a != -1 || b != -1)
            return "[0] ERR: \"" + args[1] + "\" is not a valid file name.\nUsage: $ touch filename\nUsage: $ touch filename\n\n";
        return touch(args[1]);
    }

    string save() {
        ofstream tree;
        tree.open("storage/tree.dsk");
        if ( tree.fail() ) {
            return "[0] ERR: Could not access \"storage/tree.dsk\"\n\n";
        }
        tree << root->name<<','<<root->id<<','<<root->cyl<<','<<root->sec<<'\n';
        string data = "";
        for ( Node* child : root->children )
            data += child->saveSubtree();
        tree << data;
        tree.clear();
        tree.close();
        return "[1] Tree structure saved successfully.";
    }

};

Tree* load() {
    ifstream file;
    file.open("storage/tree.dsk");
    if ( file.fail() ) {
        cout << "[0] ERR: Could not access \"storage/tree.dsk\"\n\n";
        return NULL;
    }
    string line;
    getline(file, line); // root node;

    vector<string> parts = split(line, ",");
    Node* root = new Node(parts[0]);
    root->id = stoi(parts[1]);
    root->cyl = stoi(parts[2]);
    root->sec = stoi(parts[3]);
    Tree* tree = new Tree(root);

    while( getline(file, line) ) {
        parts = split(line, ",");
        if ( parts.size() != 5 ) {
            cout << "[0] ERR: Invalid record, node discarded.\n";
            continue;  // discard any invalid records
        }
        Node* par = tree->dfsID(stoi(parts[0]));
        if ( par == NULL ) {
            cout << "[0] ERR: Child's parent not found, node discarded.\n";
            continue;
        }
        Node* child = new Node(parts[1]);
        child->id = stoi(parts[2]);
        child->cyl = stoi(parts[3]);
        child->sec = stoi(parts[4]);
        par->addChild(child);
    }
    return tree;
}

string parse(Tree* tree, vector<string> args) {    
    if ( args.size() <= 0 )
        return "";
    if ( args[0] == "pwd" )
        return tree->pwdArgs(args) + "\n\n";
    else if ( args[0] == "ls" )
        return tree->lsArgs(args);
    else if ( args[0] == "cd" )
        return tree->cdArgs(args);
    else if ( args[0] == "mkdir" )
        return tree->mkdirArgs(args);
    else if ( args[0] == "rmdir" )
        return tree->rmdirArgs(args);
    else if ( args[0] == "touch" || args[0] == "C" )
        return tree->touchArgs(args);
    else if ( args[0] == "allfiles" || args[0] == "L" )
        return tree->allFilesArgs(args);
    else
        return "[0] ERR: Command \"" + args[0] + "\" not recognized.\n\n";
}

bool prompt(Tree* tree) {
    cout << "user @ " << tree->pwd() << " $ ";
    string input;
    getline(cin, input);
    vector<string> args = split(input, " ");
    if ( args.size() > 0 && args[0] == "exit" ) {
        tree->save();
        return true;
    }
    cout << parse(tree, args);
    return false;
}

int main() {
    fileServerSocket = connectToFileServer();
    if ( fileServerSocket < 0 ) {
        cout << "Failed to connect to server.\n\n";
        return -1;
    }
    Tree* tree = load();
    bool quit = false;
    while(!quit)
        quit = prompt(tree);

}
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
using namespace std;

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

class Node {
    public:
    Node(string nombre) {
        name = nombre;
        isDir = isDirectory();
    }
    Node* parent = NULL;
    string name;
    vector<Node*> children;
    bool isDir;
    
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
            for ( Node* child : current->children )
                list += "  " + (child->name);
        }
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
                return "[0] ERR: Cannot move to a higher directory.\n\n";
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
                return "[0] ERR: \"" + c->name + "\" is not a valid directory.\n\n";
            if ( c == NULL )
                return "[0] ERR: No such directoy \"" + child + "\" exists.\n\n";
        }
        return "[1] Successfully changed directory.\n\n";
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
    
};

int main() {
    Node r("root/");
    Node a("a");
    Node b("b/");
    Node c("c/");
    r.addChild(&a);
    r.addChild(&b);
    r.addChild(&c);

    Node b1("b1/");
    Node b2("b2");

    b.addChild(&b1);
    b.addChild(&b2);

    Node c1("c1");
    Node c2("c2/");
    Node c3("c3/");
    Node c4("c4/");

    c3.addChild(&c4);
    c2.addChild(&c3);

    c.addChild(&c1);
    c.addChild(&c2);

    Tree tree(&r);

    //cout << "cd(c/c2/c3/c4) : " << tree.cd("c/c2/c3/c4") << endl;
    cout << "pwd() : " << tree.pwd() << endl;
    cout << tree.ls() << endl;
    tree.root->delChild(&b);
    cout << tree.ls() << endl;
}
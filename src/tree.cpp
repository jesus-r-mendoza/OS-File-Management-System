#include <vector>
#include <string>
#include <string.h>
#include <iostream>
using namespace std;

class Node {
    public:
    Node(string nombre, Node* par) {
        parent = par;
        name = nombre;
        isDir = isDirectory();
    }
    Node* parent;
    string name;
    vector<Node*> children;
    bool isDir;
    bool addChild(Node* child) {
        if ( child == NULL )
            return false;
        for ( Node* x : children ) {
            if ( x->name == child->name )
                return false;
        }
        child->parent = this;
        children.push_back(child);
        return true;
    }

    bool delChild(Node* child) {
        if ( child == NULL )
            return false;
        int i = 0;
        int d = -1;
        for ( Node* x : children ) {
            if ( x->name == child->name )
                d = i++;
        }
        if ( ! (d < 0) ) {
            children.erase(children.begin()+d);
            return true;
        }
        return false;
    }

    Node* getChild(Node* child) {
        if ( child == NULL )
            return NULL;
        for ( Node* x : children ) {
            if ( x->name == child->name )
                return child;
        }
        return NULL;
    }

    bool isDirectory() {
        return name[name.length()-1] == '/';
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
            cout << "has children\n";
        }
        return list;
    }
    
    string pwd() {
        Node* par = current;
        string path = "" + current->name;
	while (	par != root && par != NULL ) {
	    par = current->parent;
	    path = par->name + path;
	}
        return path;
    }
};

int main() {
    Node r("root/", NULL);
    Tree tree(&r);
    cout << tree.root->isDir << endl;
    cout << tree.current->name << endl;
    cout << tree.ls() << endl;
    cout << tree.pwd() << endl;
    /*
    cout << "- - - - - - - - - - - -\n";
    Node x;
    x.name = "x";
    Node y;
    y.name = "y";
    cout << x.addChild(&y) << endl;
    cout << x.addChild(&y) << endl;
    cout << "- - - x's children\n";
    for ( int i = 0; i < x.children.size(); i++ )
        cout << x.children[i]->name << endl;
    x.delChild(&y);
    for ( int i = 0; i < x.children.size(); i++ )
        cout << x.children[i]->name << endl;
    */
    
}

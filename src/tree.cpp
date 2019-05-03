#include <vector>
#include <string>
#include <string.h>
#include <iostream>
using namespace std;

class Node {
    public:
    Node(string nombre) {
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
        Node* par = curr->parent;
        string path = "" + curr->name;
        while ( par != NULL ) {
            curr = par;
            path = curr->name + path;
            par = curr->parent;
        }
        return path;
    }

    void cd(string child) {
        if ( child == ".." ) {
            if ( current->parent != NULL ) 
                current = current->parent;
        } else {
            Node* c = current->getChild(child);
            if ( c != NULL && c->isDir )
                current = c;
        }
    }
};

int main() {
    Node r("root/");
    Node a("a");
    Node b("b/");
    Node c("c");
    r.addChild(&a);
    r.addChild(&b);
    r.addChild(&c);

    Node b1("b1/");
    Node b2("b2");

    b.addChild(&b1);
    b.addChild(&b2);

    Tree tree(&r);
    cout << "pwd() : " << tree.pwd() << endl;
    cout << "ls() : " << tree.ls() << endl;
    cout << "cd b/ \n";
    tree.cd("b/");
    cout << "pwd() : " << tree.pwd() << endl;
    cout << "ls() : " << tree.ls() << endl;
    tree.cd("b1/");
    tree.cd("..");
    tree.cd("b2");
    //cout << "ls() : " << tree.pwd() << endl;
    /*
    cout << tree.root->isDir << endl;
    cout << tree.current->name << endl;
    cout << tree.ls() << endl;
    cout << tree.pwd() << endl;
    */


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

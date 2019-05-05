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
        return mkdir(args[1]);
    }

    string rmdirArgs(vector<string> args) {
        if ( args.size() != 2 )
            return "[0] ERR: Command \"rmdir\" requires 1 argument.\nUsage: $ rmdir dirname/\n\n";
        return rmdir(args[1]);
    }
};

string parse(Tree* tree, string input) {
    vector<string> args = split(input, " ");
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
    else
        return "[0] ERR: Command \"" + input + "\" not recognized.\n\n";
}

void prompt(Tree* tree) {
    cout << "user @ " << tree->pwd() << " $ ";
    string input;
    getline(cin, input);   
    cout << parse(tree, input);
}

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

    while(true)
        prompt(&tree);

}
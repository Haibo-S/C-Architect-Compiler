#include <iostream>
#include <vector>
#include <unordered_set>
#include <string>
#include <sstream>
using namespace std;

unordered_set<string> nonTerminalSet;

struct Node {
  // data stored at the node
  std::string data;
  // vector of pointers to child subtrees; could also use smart pointers
  std::vector<Node*> children;
  // constructor for a leaf node
  Node(std::string data) : data(data) {}
  Node(std::istream &in = std::cin) {
    // constructor that builds a tree as it reads input
  }
  // explicit destructor; could also use smart pointers
  ~Node() {
    for( auto &c : children ) { delete c; }
  }
  void print(std::string prefix, std::ostream &out = std::cout) {
    cout << data << endl;
    // Recursively call print on all children
    for (size_t i = 0; i < this->children.size(); i++) {
        if (i != this->children.size() - 1) {
            cout << prefix << "├─";
            this->children[i]->print(prefix + "│ ", out);

        }else{
            cout << prefix << "╰─";
            this->children[i]->print(prefix + "  ", out);
        }
    }
  }
};

void buildTree(Node* nonTerminal){
    string line;
    getline(cin, line);
    line.erase(0, line.find_first_not_of(' '));
    line = line.substr(line.find(' '));
    line.erase(0, line.find_first_not_of(' '));
    istringstream iss(line);
    string word;
    while (iss >> word) {
        if(word == ".EMPTY"){
            continue;
        }
        Node* newNode = new Node(word);
        nonTerminal->children.push_back(newNode);
        if(nonTerminalSet.find(word) != nonTerminalSet.end()){    
            // it is a nonterminal
            buildTree(newNode);
        }
    }
}


Node* firstBuild(){
    string line;
    getline(cin, line);
    line.erase(0, line.find_first_not_of(' '));
    Node* root = new Node(line.substr(0, line.find(' ')));
    line = line.substr(line.find(' '));
    line.erase(0, line.find_first_not_of(' '));
    istringstream iss(line);
    string word;
    while (iss >> word) {
        if(word == ".EMPTY"){
            continue;
        }
        Node* newNode = new Node(word);
        root->children.push_back(newNode);
        if(nonTerminalSet.find(word) != nonTerminalSet.end()){    
            // it is a nonterminal
            buildTree(newNode);
        }
    }
    return root;
}


int main() {
    string line;
    getline(cin, line);
    while(getline(cin, line)){
        line.erase(0, line.find_first_not_of(' '));
        if(line.find(".DERIVATION") != std::string::npos){
            break;
        }
        nonTerminalSet.insert(line.substr(0, line.find(' ')));
    }

    Node* root = firstBuild();
    root->print("");
    while(getline(cin, line)){
        if(line.find(".DERIVATION") != std::string::npos){
            delete root;
            root = firstBuild();
            root->print("");
        }
    }
    delete root;
    return 0;
}
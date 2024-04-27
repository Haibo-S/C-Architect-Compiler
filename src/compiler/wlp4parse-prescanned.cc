#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <deque>
#include <unordered_map>
using namespace std;

#include "wlp4data.h"

struct PairHash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};

struct Node {
  // data stored at the node
  string data;
  // vector of pointers to child subtrees; could also use smart pointers
  vector<Node*> children;
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
    string tempData = data;
    // if(data == ""){
    //     tempData = ".EMPTY";
    // }
    cout << tempData << endl;
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

  void print2(){
    string tempData = data;
    cout << tempData << endl;
    for (size_t i = 0; i < this->children.size(); i++) {
        this->children[i]->print2();
    }
  }
};

struct Rule{
    string LHS;
    vector<string> RHS;
    Rule(){}
};

vector<Rule> CFG;

class DFA{
    public:
        unordered_map<pair<string, string>, string, PairHash> transitions;
        unordered_map<pair<string, string>, string, PairHash> reductions;
    DFA(){};
    void printTransitions() {
        for (const auto& entry : transitions) {
            const auto& key = entry.first;
            const auto& value = entry.second;
            std::cout << "(" << key.first << ", " << key.second << ") -> " << value << std::endl;
        }
    }
    void printReductions() {
        for (const auto& entry : reductions) {
            const auto& key = entry.first;
            const auto& value = entry.second;
            std::cout << "(" << key.first << ", " << key.second << ") -> " << value << std::endl;
        }
    }
};

deque<string> inputs;

vector<Node*> treeStack;
vector<string> stateStack;

void readData(DFA &dfa);
void reduceTrees(Rule &r);
void reduceStates(Rule &r, DFA& dfa);
void shift(DFA& dfa);
void tokensToTrees(string s);
void parse(DFA& dfa);
void printStateStack();
void printTreeStack();

void printStateStack(){
    cout << "The current stateStack is: " << endl;
    for(size_t i = 0; i < stateStack.size(); i++){
        cout << stateStack[i] << " ";
    }
    cout << endl;
    cout << endl;
}

void printTreeStack(){
    cout << "The current treeStack is: " << endl;
    for(size_t i = 0; i < treeStack.size(); i++){
        treeStack[i]->print("");
    }
    cout << endl;
}


int main(){
    DFA dfa;
    try{
        readData(dfa);
        parse(dfa);
        // treeStack.back()->print("");
        treeStack.back()->print2();

    }catch(std::runtime_error &e) {
        for (Node* node : treeStack) {
            delete node;
        }
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
    for (Node* node : treeStack) {
        delete node;
    }
    
    
    // for(size_t i = 0; i < inputs.size(); i++){
    //     cout << inputs[i] << endl;
    // }
    // dfa.printTransitions();
    // tokensToTrees("a b c d");
    // cout << "Before Reduce: " << endl;
    // for(size_t i = 0; i < treeStack.size(); i++){
    //     cout << "Element " << i << endl;
    //     treeStack[i]->print("");
    // }
    // cout << endl;
    // cout << "After Reduce: " << endl;
    // Rule r;
    // r.LHS = "dcls";
    // r.RHS = {".EMPTY"};
    // Rule r;
    // r.LHS = "start";
    // r.RHS = {"BOF", "procedures", "BOF"};
    // reduceTrees(r);
    // for(size_t i = 0; i < treeStack.size(); i++){
    //     cout << "Element " << i << endl;
    //     treeStack[i]->print("");
    // }
    return 0;
}

void parse(DFA& dfa){
    while(inputs.size() > 0){
        string symbol = inputs.front().substr(0, inputs.front().find(' '));
        // cout << symbol << endl;
        while(dfa.reductions.find({stateStack.back(), symbol}) != dfa.reductions.end()){
            Rule r = CFG[stoi(dfa.reductions[{stateStack.back(), symbol}])];
            // cout << "Reduction State" << endl;
            // cout << stoi(dfa.reductions[{stateStack.back(), symbol}]) << endl;
            // if(stoi(dfa.reductions[{stateStack.back(), symbol}]) == 11){
            //     cout << "Inside 11" << endl;
            //     printStateStack();
            // }
            reduceTrees(r);
            reduceStates(r, dfa);
            // printStateStack();
            // printTreeStack();

            // inputs.pop_front();
            // if(inputs.size() <= 0){
            //     break;
            // }
            // symbol = inputs.front().substr(0, inputs.front().find(' '));
        }
        // if(inputs.size() <= 0){
        //     break;
        // }
        shift(dfa);
    }
    reduceTrees(CFG[0]);

}

// void tokensToTrees(string s){
//     istringstream iss(s);
//     string data;
//     while(iss >> data){
//         Node* newNode = new Node("ID "+data);
//         treeStack.push_back(newNode);
//     }

// }

void reduceTrees(Rule &r){
    string rule_data = r.LHS;
    // if(r.LHS != "start"){
    for(const auto& data: r.RHS){
        if(data == ""){
            rule_data += " .EMPTY";
        }else{
            rule_data += " " + data;
        }
        
    }
    // }
    
    Node* newNode = new Node(rule_data);
    size_t len = r.RHS[0] == "" ? 0 : r.RHS.size();
    for(size_t i = 0; i < len; i++){
        newNode->children.insert(newNode->children.begin(), treeStack[treeStack.size()-1]);
        treeStack.pop_back();
    }
    treeStack.push_back(newNode);
    
}

void reduceStates(Rule &r, DFA& dfa){
    size_t len = r.RHS[0] == "" ? 0 : r.RHS.size();
    for(size_t i = 0; i < len; i++){
        stateStack.pop_back();
    }
    // if(len != 0){
    stateStack.push_back(dfa.transitions[{stateStack.back(), r.LHS}]);
    // }
    
}

void shift(DFA& dfa){
    Node* newNode = new Node(inputs.front());
    // cout << inputs.front() << endl;
    string symbol = newNode->data.substr(0, newNode->data.find(' '));
    if(dfa.transitions.find({stateStack.back(), symbol}) != dfa.transitions.end()){
        // cout << dfa.transitions[{stateStack.back(), symbol}] << endl;
        stateStack.push_back(dfa.transitions[{stateStack.back(), symbol}]);
        treeStack.push_back(newNode);
        inputs.pop_front();
    }else{
        // cout << stateStack.back() << endl;
        // cout << symbol << endl;
        delete newNode;
        throw runtime_error("Error: No such transition exists.");
    }
    
}


void readData(DFA &dfa){
    stringstream cfg_data(WLP4_CFG);
    string line;
    getline(cfg_data, line);
    while(getline(cfg_data, line)){
        Rule newRule;
        newRule.LHS = line.substr(0, line.find(' '));
        istringstream iss(line.substr(line.find(' ')+1));
        string temp;
        while(iss >> temp){
            if(temp == ".EMPTY"){
                temp = "";
            }
            newRule.RHS.push_back(temp);
        }
        CFG.push_back(newRule);
    }

    stringstream transitions_rules(WLP4_TRANSITIONS);
    getline(transitions_rules, line);
    while(getline(transitions_rules, line)){
        istringstream iss(line);
        string from;
        iss >> from;
        string symbol;
        iss >> symbol;
        string to;
        iss >> to;
        dfa.transitions[{from, symbol}] = to;
    }

    stringstream reductions_rules(WLP4_REDUCTIONS);
    getline(reductions_rules, line);
    while(getline(reductions_rules, line)){
        istringstream iss(line);
        string state;
        iss >> state;
        string rule_number;
        iss >> rule_number;
        string symbol;
        iss >> symbol;
        dfa.reductions[{state, symbol}] = rule_number;
    }

    while(getline(cin, line)){
        inputs.push_back(line);
    }
    inputs.push_front("BOF BOF");
    inputs.push_back("EOF EOF");

    stateStack.push_back("0");
}
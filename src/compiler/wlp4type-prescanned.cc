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
  // left hand side of the rule
  string left;
  // type of the expression
  string type;
  // vector of pointers to child subtrees; could also use smart pointers
  vector<Node*> children;
  // constructor for a leaf node
  Node(std::string data) : data(data) {left = data.substr(0,data.find(' '));}
  Node(std::istream &in = std::cin) {
    // constructor that builds a tree as it reads input
  }
  // explicit destructor; could also use smart pointers
  ~Node() {
    for( auto &c : children ) { delete c; }
  }

  Node* getChild(string str, int n){
    for(size_t i = 0; i < children.size(); i++){
        if(children[i]->left == str){
            if(n == 1){
                return children[i];
            }
            n--;
        }
    }
    return nullptr;
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

struct Variable{
        Variable(Node* root){
            this->helper(root);
        };
        Variable(){}
        void helper(Node* root){
            if(root->left != "dcl"){
                throw runtime_error("Error: DCL not on the left");
            }
            this->name = root->children[1]->data;
            this->type = root->children[0]->children.size() == 1 ? "INT" : "INT STAR";
        }

        string name;
        string type;

        void print() const {
            cout << "Variable Name: " << name << ", Type: " << type << endl;
        }

        ~Variable() {}
};

struct VariableTable{
        VariableTable(){
            variableMap = {};
        };

        unordered_map<string, Variable> variableMap;

        void add(Variable var){
            if(this->variableMap.find(var.name) != variableMap.end()){
                throw runtime_error("Error: duplicate variable declaration");
            }
            variableMap[var.name] = var;
        }
        Variable get(string checkName){
            if(this->variableMap.find(checkName) != variableMap.end()){
                return variableMap[checkName];
            }else{
                throw runtime_error("Error: use of undeclared variable");
            }
        }
        void print() const {
            for (auto& pair : variableMap) {
                cout << "Variable Key: " << pair.first << " -> ";
                pair.second.print();
            }
        }
};


struct Procedure{
        Procedure(Node* root){
            this->helper(root);
        };
        Procedure(){}

        string name;
        vector<string> signature;
        VariableTable variable_table;

        void helper(Node* root){
            signature = {};
            variable_table = {};
            if(root->left != "procedure" && root->left != "main"){
                throw runtime_error("Error: procedure or main not on the left");
            }
            signature = vector<string>();
            variable_table = VariableTable();
            if(root->left == "main"){
                //size_t spaceIndex = root->getChild("WAIN", 1)->data.find(' ');
                //name = root->getChild("WAIN", 1)->data.substr(spaceIndex+1);
                name = root->getChild("WAIN", 1)->data;
                Variable first (root->getChild("dcl", 1));
                Variable second (root->getChild("dcl", 2));
                variable_table.add(first);
                variable_table.add(second);
                signature.push_back(first.type);
                signature.push_back(second.type);
            }else{
                //size_t spaceIndex = root->getChild("ID", 1)->data.find(' ');
                //name = root->getChild("ID", 1)->data.substr(spaceIndex+1);
                name = root->getChild("ID", 1)->data;
                extractProcedureSignature(root->getChild("params", 1));
            }
            extractVariables(root->getChild("dcls", 1));
        }

        void extractProcedureSignature(Node* root){
            if(root->left == "dcl"){
                Variable newVariable (root);
                signature.push_back(newVariable.type);
                variable_table.add(newVariable);
            }
            for(size_t i = 0; i < root->children.size(); i++){
                extractProcedureSignature(root->children[i]);
            }
        }

        void extractVariables(Node* root){
            if (root->data == "dcls dcls dcl BECOMES NUM SEMI") {
                Variable newVariable (root->getChild("dcl",1));
                if(newVariable.type != "INT"){
                    throw runtime_error("ERROR: dcls dcls dcl BECOMES NUM SEMI");
                }
                variable_table.add(newVariable);
                for(size_t i = 0; i < root->children.size(); i++){
                    extractVariables(root->children[i]);
                }
            }else if(root->data == "dcls dcls dcl BECOMES NULL SEMI"){
                Variable newVariable (root->getChild("dcl",1));
                if(newVariable.type != "INT STAR"){
                    throw runtime_error("dcls dcls dcl BECOMES NULL SEMI");
                }
                variable_table.add(newVariable);
                for(size_t i = 0; i < root->children.size(); i++){
                    extractVariables(root->children[i]);
                }
            }else{
                for(size_t i = 0; i < root->children.size(); i++){
                    extractVariables(root->children[i]);
                }
            }
        }

        void print() const {
            cout << "Procedure Name: " << name << endl;
            cout << "Signature: ";
            for (const auto& type : signature) {
                cout << type << " ";
            }
            cout << endl;
            cout << "Variables: " << endl;
            variable_table.print();
        }


        
};

struct ProcedureTable{
        ProcedureTable(){
            procedureMap = {};
        };

        unordered_map<string, Procedure> procedureMap;

        void add(Procedure pro){
            if(this->procedureMap.find(pro.name) != procedureMap.end()){
                throw runtime_error("Error: duplicate procedure declaration");
            }
            this->procedureMap[pro.name] = pro;
        }
        Procedure get(string checkName){
            if(this->procedureMap.find(checkName) != procedureMap.end()){
                return procedureMap[checkName];
            }else{
                throw runtime_error("Error: use of procedure variable");
            }
        }
        void print() const {
            for (auto& pair : procedureMap) {
                cout << "Procedure Key: " << pair.first << " -> ";
                pair.second.print();
            }
        }
};

// lists of methods
void collectProcedures(Node* root, ProcedureTable& table);
void annotateTypes(Node* root, Procedure curProc, ProcedureTable& table);
void moreTypeErrors(ProcedureTable table);
void checkStatements(Node* root);


int main(){
    DFA dfa;
    ProcedureTable table;
    try{
        readData(dfa);
        parse(dfa);
        // treeStack.back()->print("");

        Node* head = treeStack.back();
        table = ProcedureTable();
        collectProcedures(head, table);
        // table->print();

        for (Node* node : treeStack) {
            delete node;
        }
        

    }catch(std::runtime_error &e) {
        for (Node* node : treeStack) {
            delete node;
        }
        
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}


void annotateTypes(Node* root, Procedure curProc, ProcedureTable& table){
    for(size_t i = 0; i < root->children.size(); i++){
        annotateTypes(root->children[i], curProc, table);
    }

    if(root->data == "factor NUM"){
        root->type = "INT";
    }else if(root->data == "factor NULL"){
        root->type = "INT STAR";
    }else if(root->data == "factor ID"){
        Variable temp = curProc.variable_table.get(root->getChild("ID",1)->data);
        if(temp.type != "INT" && temp.type != "INT STAR"){
             throw runtime_error("Error: factor ID.");
        }
        root->type = temp.type;
    }else if(root->data == "lvalue ID"){
        Variable temp = curProc.variable_table.get(root->getChild("ID",1)->data);
        if(temp.type != "INT" && temp.type != "INT STAR"){
            throw runtime_error("Error: lvalue ID.");
        }
        root->type = temp.type;
    }else if(root->data == "factor LPAREN expr RPAREN"){
        root->type = root->getChild("expr",1)->type;
    }else if(root->data == "factor lvalue expr RPAREN"){
        root->type = root->getChild("lvalue",1)->type;
    }else if(root->data == "factor AMP lvalue"){
        if(root->getChild("lvalue",1)->type != "INT"){
            throw runtime_error("Error: factor AMP lvalue. getchild(ID) output not INT");
        }
        root->type = "INT STAR";
    }else if(root->data == "factor STAR factor"){
        if(root->getChild("factor",1)->type != "INT STAR"){
            throw runtime_error("Error: factor STAR factor. getchild(ID) output not INT STAR");
        }
        root->type = "INT";
    }else if(root->data == "lvalue STAR factor"){
        if(root->getChild("factor",1)->type != "INT STAR"){
            throw runtime_error("Error: lvalue STAR factor. getchild(ID) output not INT STAR");
        }
        root->type = "INT";
    }else if(root->data == "lvalue LPAREN lvalue RPAREN"){
        root->type = root->getChild("lvalue",1)->type;
    }else if(root->data == "factor NEW INT LBRACK expr RBRACK"){
        if(root->getChild("expr",1)->type != "INT"){
            throw runtime_error("Error: factor NEW INT LBRACK expr RBRACK. getchild(ID) output not INT");
        }
        root->type = "INT STAR";
    }else if(root->data == "factor ID LPAREN RPAREN"){
        if(curProc.variable_table.variableMap.find(root->getChild("ID", 1)->data) != curProc.variable_table.variableMap.end()){
            throw runtime_error("Error: factor ID LPAREN arglist RPAREN. ID defined in current procedure");
        }
        if(table.get(root->getChild("ID", 1)->data).signature.size() != 0){
            throw runtime_error("Error: factor ID LPAREN RPAREN. ID signature size not zero");
        }
        root->type = "INT";
    }else if(root->data == "factor ID LPAREN arglist RPAREN"){
        if(curProc.variable_table.variableMap.find(root->getChild("ID", 1)->data) != curProc.variable_table.variableMap.end()){
            throw runtime_error("Error: factor ID LPAREN arglist RPAREN. ID defined in current procedure");
        }
        // no need to check if ID procedure exists in tree since table->get outputs an error if such doesn't exist
        Procedure cur_procedure = table.get(root->getChild("ID", 1)->data); 
        vector<string> correct_signature = cur_procedure.signature;
        // we also need to find the # of arguments in arglists
        // provided_signature stores the types of inputs provided by arglist
        vector<string> provided_signature;
        Node* cur = root->getChild("arglist",1);
        size_t count_signature = 0;

        while(cur != NULL){
            // shouldn't worry about whether that argument exists or not. cur_procedure->variable_table->get() output error if such doesn't exist
            string arg_type = cur->getChild("expr",1)->type;
            provided_signature.push_back(arg_type);
            count_signature++;
            cur = cur->getChild("arglist",1);
        }

        // we now check whether count_signature == correct_signature.size()
        if(count_signature != correct_signature.size()){
            throw runtime_error("Error: factor ID LPAREN arglist RPAREN. input argument number != correct argument number");
        }

        // precondition: count_signature = correct_signature.size(); iterate the two vector to check for types
        for(size_t i = 0; i < count_signature; i++){
            if(provided_signature[i] != correct_signature[i]){
                cout << "provided_signature at index " << i << " is: " << provided_signature[i] << endl;
                cout << "correct_signature at index " << i << " is: " << correct_signature[i] << endl;
                throw runtime_error("Error: factor ID LPAREN arglist RPAREN. provided_signature[i] != correct_signature[i]");
            }
        }

        root->type = "INT";
    }else if(root->data == "term factor"){
        root->type = root->getChild("factor",1)->type;
    }else if(root->data == "term term STAR factor" || root->data == "term term SLASH factor" || root->data == "term term PCT factor"){
        if(root->getChild("term",1)->type != "INT" || root->getChild("factor",1)->type != "INT"){
            throw runtime_error("Error: term term STAR factor. term or factor not type INT");
        }
        root->type = "INT";
    }else if(root->data == "expr term"){
        root->type = root->getChild("term",1)->type;
    }else if(root->data == "expr expr PLUS term"){
        if(root->getChild("expr",1)->type == "INT" && root->getChild("term",1)->type == "INT"){
            root->type = "INT";
        }else if(root->getChild("expr",1)->type == "INT STAR" && root->getChild("term",1)->type == "INT"){
            root->type = "INT STAR";
        }else if(root->getChild("expr",1)->type == "INT" && root->getChild("term",1)->type == "INT STAR"){
            root->type = "INT STAR";
        }else{
            throw runtime_error("Error: expr expr PLUS term. expr and term invalid type matches");
        }
    }else if(root->data == "expr expr MINUS term"){
        if(root->getChild("expr",1)->type == "INT" && root->getChild("term",1)->type == "INT"){
            root->type = "INT";
        }else if(root->getChild("expr",1)->type == "INT STAR" && root->getChild("term",1)->type == "INT"){
            root->type = "INT STAR";
        }else if(root->getChild("expr",1)->type == "INT STAR" && root->getChild("term",1)->type == "INT STAR"){
            root->type = "INT";
        }else{
            throw runtime_error("Error: expr expr MINUS term. expr and term invalid type matches");
        }
    }
}




void collectProcedures(Node* root, ProcedureTable& table){
    if(root->left == "procedure" || root->left == "main"){
        Procedure newProc (root);
        table.add(newProc);
        annotateTypes(root, newProc, table);

        if(root->left == "main"){
            if(newProc.signature.size() != 2){
                throw runtime_error("Error: More Type Errors. WAIN no second dcl");
            }
            if(newProc.signature[1] != "INT"){
                throw runtime_error("Error: More Type Errors. WAIN second dcl not INT");
            }     
        }

        if(root->getChild("expr",1)->type != "INT"){
            throw runtime_error("Error: More Type Errors. procedure expr not INT");
        }

        checkStatements(root);
        
    }else{
        for(size_t i = 0; i < root->children.size(); i++){
            collectProcedures(root->children[i], table);
        }
    }
    
}


void checkStatements(Node* root){
    for(size_t i = 0; i < root->children.size(); i++){
        checkStatements(root->children[i]);
    }

    if(root->data == "statement lvalue BECOMES expr SEMI" && root->getChild("lvalue",1)->type != root->getChild("expr",1)->type){
            throw runtime_error("Error: More Type Errors. statement lvalue BECOMES expr SEMI. lvalue type != expr type");
        }
        
    if(root->data == "statement PRINTLN LPAREN expr RPAREN SEMI" && root->getChild("expr",1)->type != "INT"){
        throw runtime_error("Error: More Type Errors. statement PRINTLN LPAREN expr RPAREN SEMI. expr type not INT");
    }

    if(root->data == "statement DELETE LBRACK RBRACK expr SEMI" && root->getChild("expr",1)->type != "INT STAR"){
        throw runtime_error("Error: More Type Errors. statement DELETE LBRACK RBRACK expr SEMI. expr type not INT STAR");
    }

    if(root->data == "test expr EQ expr" || root->data == "test expr NE expr" || root->data == "test expr LT expr" ||
    root->data == "test expr LE expr" || root->data == "test expr GE expr" || root->data == "test expr GT expr"){
        if(root->getChild("expr",1)->type != root->getChild("expr",2)->type){
            throw runtime_error("Error: More Type Errors. test expr XX expr. expr 1 type not equal expr 2 type");
        }
    }
}


// some fucking paring algorithm

void parse(DFA& dfa){
    while(inputs.size() > 0){
        string symbol = inputs.front().substr(0, inputs.front().find(' '));
        while(dfa.reductions.find({stateStack.back(), symbol}) != dfa.reductions.end()){
            Rule r = CFG[stoi(dfa.reductions[{stateStack.back(), symbol}])];
            reduceTrees(r);
            reduceStates(r, dfa);
        }
        shift(dfa);
    }
    reduceTrees(CFG[0]);

}

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
    string symbol = newNode->data.substr(0, newNode->data.find(' '));
    if(dfa.transitions.find({stateStack.back(), symbol}) != dfa.transitions.end()){
        stateStack.push_back(dfa.transitions[{stateStack.back(), symbol}]);
        treeStack.push_back(newNode);
        inputs.pop_front();
    }else{
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
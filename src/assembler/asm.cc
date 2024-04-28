#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <unordered_map>
#include <cstdlib>
#include <regex>
#include <cctype>
#include <math.h>
using namespace std;

//// The dfa.h header defines a string constant containing a DFA file:
// std::string DFAstring = ...
#include "dfa.h"

// bool decisinrange checks if the input is in range
bool isWithinRange(const std::string& num, string low_bound, string up_bound);
// bool hexisinrange checks is the input is in range
bool hexIsWithinRange(const std::string& num, string bound);
// bool reg is in range

struct PairHash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        return h1 ^ h2;
    }
};

class DFA{
  public:
      unordered_map<string, bool> dfaStates;
      unordered_map<pair<string, char>, string, PairHash> nextStates;
  DFA(){};
  string getNextState(string state, char input){
    auto it = this->nextStates.find({state, input});
    if(it != this->nextStates.end()){
      return nextStates[{state, input}];
    }else{
      return "8964";
    }
  }
};

class Assembler{
  private:
    void buildIDMap() {
      IDMap["add"] = 32;
      IDMap["sub"] = 34;
      IDMap["mult"] = 24;
      IDMap["multu"] = 25;
      IDMap["div"] = 26;
      IDMap["divu"] = 27;
      IDMap["mfhi"] = 16;
      IDMap["mflo"] = 18;
      IDMap["lis"] = 20;
      IDMap["lw"] = 35;
      IDMap["sw"] = 43;
      IDMap["slt"] = 42;
      IDMap["sltu"] = 43;
      IDMap["beq"] = 4;
      IDMap["bne"] = 5;
      IDMap["jr"] = 8;
      IDMap["jalr"] = 9;
    }
  public:
      vector<vector<string>> lines;
      unordered_map<string, int> symbol_map;
      unordered_map<string, int> IDMap;
  Assembler(){
    buildIDMap();
  };

  void fillAss(vector<string> list) {
    size_t N = list.size();
    vector<string> temp;
    for (size_t i = 0; i < N; i++) {
      if (list[i] != "NEWLINE") {
        temp.push_back(list[i]);
      } else {
        if (!temp.empty() && temp.back().find("LABELDEF") == 0 && i + 1 < N) {
          bool allNewlines = true;
          size_t j = i + 1;
          while (j < N && list[j] == "NEWLINE") {
            j++;
          }
          if (j < N && list[j] != "NEWLINE") {
            allNewlines = false;
          }

          if (!allNewlines) {
            continue;
          }
        }
        if (!temp.empty()) {
          this->lines.push_back(temp);
          temp.clear();
        }
      }
    }

    if (!temp.empty()) {
      this->lines.push_back(temp);
    }
  }

  void fillAssTable(){
    size_t N = this->lines.size();
    for(size_t i = 0; i < N; i++){
      for(size_t j = 0; j < this->lines[i].size(); j++){
        string current = this->lines[i][j];
        int currentSize = current.length();
        if(current.find("LABELDEF") == 0){
          string labelName = current.substr(9, currentSize - 9 - 1);
          if(this->symbol_map.find(labelName) == symbol_map.end()){
            this->symbol_map[labelName] = i * 4;
          }else{
            throw runtime_error("Duplicate Label Error: " + labelName + "\n");
          }
        }
      }
    }
  }


  void analysisCheck(){
    size_t N = this->lines.size();
    for(size_t i = 0; i < N; i++){
      for(size_t j = 0; j < this->lines[i].size(); j++){
        string current = this->lines[i][j];
        if(current.substr(0, current.find(' ')) == "LABELDEF"){
          continue;
        }

        //int currentSize = current.length();
        if(current.substr(0, current.find(' ')) == "ID"){
          string IDName = current.substr(3);
          // cout << IDName << endl;
          if(this->IDMap.find(IDName) == this->IDMap.end()){
            throw runtime_error("Invalid Opcode Error: " + current + "\n");
          }
          
          if(IDName == "add" || IDName == "sub" || IDName == "slt" || IDName == "sltu"){
            // three register operands
            // cout << "Inside three register operand: " + IDName + "\n";
            if(j + 6 != this->lines[i].size()){
              throw runtime_error("Three register operand format mismatch: " + IDName + "\n");
            }
            string tempStr = this->lines[i][j+1];
            if(tempStr.substr(0, tempStr.find(' ')) != "REGISTER"){
              throw runtime_error("Three register operand 1st register mismatch: " + IDName + "\n");
            }
            tempStr = this->lines[i][j+2];
            if(tempStr.substr(0, tempStr.find(' ')) != "COMMA"){
              throw runtime_error("Three register operand 2nd comma mismatch: " + IDName + "\n");
            }
            tempStr = this->lines[i][j+3];
            if(tempStr.substr(0, tempStr.find(' ')) != "REGISTER"){
              throw runtime_error("Three register operand 3rd register mismatch: " + IDName + "\n");
            }
            tempStr = this->lines[i][j+4];
            if(tempStr.substr(0, tempStr.find(' ')) != "COMMA"){
              throw runtime_error("Three register operand 4th comma mismatch: " + IDName + "\n");
            }
            tempStr = this->lines[i][j+5];
            if(tempStr.substr(0, tempStr.find(' ')) != "REGISTER"){
              throw runtime_error("Three register operand 5th register mismatch: " + IDName + "\n");
            }
            j += 6;
          }else if(IDName == "mult" || IDName == "multu" || IDName == "div" || IDName == "divu"){
            // two register operands
            if(j + 4 != this->lines[i].size()){
              throw runtime_error("Two register operand format mismatch: " + IDName + "\n");
            }
            string tempStr = this->lines[i][j+1];
            if(tempStr.substr(0, tempStr.find(' ')) != "REGISTER"){
              throw runtime_error("Two register operand 1st register mismatch: " + IDName + "\n");
            }
            tempStr = this->lines[i][j+2];
            if(tempStr.substr(0, tempStr.find(' ')) != "COMMA"){
              throw runtime_error("Two register operand 2nd comma mismatch: " + IDName + "\n");
            }
            tempStr = this->lines[i][j+3];
            if(tempStr.substr(0, tempStr.find(' ')) != "REGISTER"){
              throw runtime_error("Two register operand 3rd register mismatch: " + IDName + "\n");
            }
            j += 4;
          }else if(IDName == "beq" || IDName == "bne"){
            // two registers, one immediate
            if(j + 6 != this->lines[i].size()){
              throw runtime_error("Branch Instructions operand format mismatch: " + IDName + "\n");
            }
            string tempStr = this->lines[i][j+1];
            if(tempStr.substr(0, tempStr.find(' ')) != "REGISTER"){
              throw runtime_error("Branch Instructions operand 1st register mismatch: " + IDName + "\n");
            }
            tempStr = this->lines[i][j+2];
            if(tempStr.substr(0, tempStr.find(' ')) != "COMMA"){
              throw runtime_error("Branch Instructions operand 2nd comma mismatch: " + IDName + "\n");
            }
            tempStr = this->lines[i][j+3];
            if(tempStr.substr(0, tempStr.find(' ')) != "REGISTER"){
              throw runtime_error("Branch Instructions operand 3rd register mismatch: " + IDName + "\n");
            }
            tempStr = this->lines[i][j+4];
            if(tempStr.substr(0, tempStr.find(' ')) != "COMMA"){
              throw runtime_error("Branch Instructions operand 4th comma mismatch: " + IDName + "\n");
            }
            tempStr = this->lines[i][j+5];
            string s = tempStr.substr(0, tempStr.find(' '));
            if(s != "DECINT" && s != "HEXINT" && s != "ID"){
              throw runtime_error("Branch Instructions operand 5th DECINT / HEXINT / ID mismatch: " + IDName + "\n");
            }
            if(s == "HEXINT"){
              string hexNum = tempStr.substr(tempStr.find(' ')+1);
              if(!hexIsWithinRange(hexNum, "0xFFFF")){
                throw runtime_error("Branch Instructions HEXINT out of range 0xFFFF: " + hexNum + "\n");
              }
            }else if(s == "DECINT"){
              string decNum = tempStr.substr(tempStr.find(' ')+1);
              if(!isWithinRange(decNum, "-32768", "32767")){
                throw runtime_error("Branch Instructions DECINT out of range -32768 to 32767: " + decNum + "\n");
              }
            }else{
              // check for non-existence label
              string tempLabel = tempStr.substr(tempStr.find(' ')+1);
              if(this->symbol_map.find(tempLabel) == this->symbol_map.end()){
                throw runtime_error("Branch Instructions - Label not existence error: " + tempLabel + "\n");
              }
              // check for opcode over-range error
              long labelValue = this->symbol_map[tempLabel];
              long curPC = (i+1) * 4;
              long opcode = (labelValue - curPC) / 4;
              string decInt = to_string(opcode);
              if(!isWithinRange(decInt, "-32768", "32767")){
                throw runtime_error("Branch Instructions - label opcode out of range -32768 to 32767: " + decInt + "\n");
              }
            }
            j += 6;
          }else if(IDName == "lw" || IDName == "sw"){
            // register immediate register
            if(j + 7 != this->lines[i].size()){
              throw runtime_error("Memory Access Instructions operand format mismatch: " + IDName + "\n");
            }
            string tempStr = this->lines[i][j+1];
            if(tempStr.substr(0, tempStr.find(' ')) != "REGISTER"){
              throw runtime_error("Memory Access Instructions operand 1st register mismatch: " + IDName + "\n");
            }
            tempStr = this->lines[i][j+2];
            if(tempStr.substr(0, tempStr.find(' ')) != "COMMA"){
              throw runtime_error("Memory Access Instructions operand 2nd comma mismatch: " + IDName + "\n");
            }
            tempStr = this->lines[i][j+3];
            if(tempStr.substr(0, tempStr.find(' ')) != "DECINT" && tempStr.substr(0, tempStr.find(' ')) != "HEXINT"){
              throw runtime_error("Memory Access Instructions operand 3rd DECINT / HEXINT mismatch: " + IDName + "\n");
            }
            if(tempStr.substr(0, tempStr.find(' ')) == "HEXINT"){
              string hexNum = tempStr.substr(tempStr.find(' ')+1);
              if(!hexIsWithinRange(hexNum, "0xFFFF")){
                throw runtime_error("Memory Access Instructions HEXINT out of range 0xFFFF: " + hexNum + "\n");
              }
            }
            if(tempStr.substr(0, tempStr.find(' ')) == "DECINT"){
              string decNum = tempStr.substr(tempStr.find(' ')+1);
              if(!isWithinRange(decNum, "-32768", "32767")){
                throw runtime_error("Memory Access Instructions DECINT out of range -32768 to 32767: " + decNum + "\n");
              }
            }
            tempStr = this->lines[i][j+4];
            if(tempStr.substr(0, tempStr.find(' ')) != "LPAREN"){
              throw runtime_error("Memory Access Instructions operand 4th LPAREN mismatch: " + IDName + "\n");
            }
            tempStr = this->lines[i][j+5];
            if(tempStr.substr(0, tempStr.find(' ')) != "REGISTER"){
              throw runtime_error("Memory Access Instructions operand 5th REGISTER mismatch: " + IDName + "\n");
            }
            tempStr = this->lines[i][j+6];
            if(tempStr.substr(0, tempStr.find(' ')) != "RPAREN"){
              throw runtime_error("Memory Access Instructions operand 6th RPAREN mismatch: " + IDName + "\n");
            }
            j += 7;
          }else{
            // one register operand
            if(j + 2 != this->lines[i].size()){
              throw runtime_error("One register operand format mismatch: " + IDName + "\n");
            }
            string tempStr = this->lines[i][j+1];
            if(tempStr.substr(0, tempStr.find(' ')) != "REGISTER"){
              throw runtime_error("One register operand 1st register mismatch: " + IDName + "\n");
            }
            j += 2;
          }

        }else if(current.substr(0, current.find(' ')) == "DOTID"){
          if(j + 2 != this->lines[i].size()){
            throw runtime_error(".word format mismatch \n");
          }
          string nextStr = this->lines[i][j+1];
          string nextStrType = nextStr.substr(0, nextStr.find(' '));
          if(nextStrType != "ID" && nextStrType != "DECINT" && nextStrType != "HEXINT"){
            throw runtime_error(".word next element mismatch \n");
          }
          if(nextStrType == "ID"){
            string IDName = nextStr.substr(3);
            if(this->symbol_map.find(IDName) == this->symbol_map.end()){
              throw runtime_error("Wrong ID Name Error: " + nextStr + "\n");
            }
          }
          j += 2;
        }else{
          throw runtime_error("Invalid Opcode Error: " + current + "\n");
        }
      }
    }
  }

  void synthesisEncoding(){
    size_t N = this->lines.size();
    for(size_t i = 0; i < N; i++){
      for(size_t j = 0; j < this->lines[i].size(); j++){
        string current = this->lines[i][j];
        if(current.substr(0, current.find(' ')) == "LABELDEF"){
          continue;
        }
        if(current.substr(0, current.find(' ')) == "ID"){
          string IDName = current.substr(3);
          if(IDName == "add" || IDName == "sub" || IDName == "slt" || IDName == "sltu"){
            // three register operands
            string tempStr = this->lines[i][j+1];
            int d = stoi(tempStr.substr(tempStr.find(' ')+2));
            tempStr = this->lines[i][j+3];
            int s = stoi(tempStr.substr(tempStr.find(' ')+2));
            tempStr = this->lines[i][j+5];
            int t = stoi(tempStr.substr(tempStr.find(' ')+2));
            int encode = s << 21 | t << 16 | d << 11 | IDMap[current.substr(current.find(' ')+1)];
            printEncode(encode);
            j += 6;
          }else if(IDName == "mult" || IDName == "multu" || IDName == "div" || IDName == "divu"){
            // two register operands
            string tempStr = this->lines[i][j+1];
            int s = stoi(tempStr.substr(tempStr.find(' ')+2));
            tempStr = this->lines[i][j+3];
            int t = stoi(tempStr.substr(tempStr.find(' ')+2));
            int encode = s << 21 | t << 16 | IDMap[current.substr(current.find(' ')+1)];
            printEncode(encode);
            j += 4;
          }else if(IDName == "beq" || IDName == "bne"){
            // branch instructions
            string tempStr = this->lines[i][j+1];
            int s = stoi(tempStr.substr(tempStr.find(' ')+2));
            tempStr = this->lines[i][j+3];
            int t = stoi(tempStr.substr(tempStr.find(' ')+2));
            tempStr = this->lines[i][j+5];
            string ident = tempStr.substr(0, tempStr.find(' '));
            int op;
            if(ident == "DECINT"){
              op = stoi(tempStr.substr(tempStr.find(' ')+1));
            }else if(ident == "HEXINT"){
              op = stoul(tempStr.substr(tempStr.find(' ')+1), nullptr, 16);
            }else{
              int labelValue = this->symbol_map[tempStr.substr(tempStr.find(' ')+1)];
              int curPC = (i+1) * 4;
              op = (labelValue - curPC) / 4;
            }
            if(op < 0){
              op &= 65535;
            }
            int encode = IDMap[current.substr(current.find(' ')+1)] << 26 | s << 21 | t << 16 | op;
            printEncode(encode);
            j += 6;
          }else if(IDName == "lw" || IDName == "sw"){
            // register immediate operand
            string tempStr = this->lines[i][j+1];
            int t = stoi(tempStr.substr(tempStr.find(' ')+2));
            tempStr = this->lines[i][j+3];
            int op;
            if(tempStr.substr(0, tempStr.find(' ')) == "DECINT"){
              op = stoi(tempStr.substr(tempStr.find(' ')+1));
            }else if(tempStr.substr(0, tempStr.find(' ')) == "HEXINT"){
              op = stoul(tempStr.substr(tempStr.find(' ')+1), nullptr, 16);
            }
            if(op < 0){
              op &= 65535;
            }
            tempStr = this->lines[i][j+5];
            int s = stoi(tempStr.substr(tempStr.find(' ')+2));
            int encode = IDMap[current.substr(current.find(' ')+1)] << 26 | s << 21 | t << 16 | op;
            printEncode(encode);
            j += 7;
          }else{
            // single register operand
            string tempStr = this->lines[i][j+1];
            int d = stoi(tempStr.substr(tempStr.find(' ')+2));
            int encode;
            if(IDName == "lis" || IDName == "mflo" || IDName == "mfhi"){
              encode = d << 11 | IDMap[current.substr(current.find(' ')+1)];
            }else{
              encode = d << 21 | IDMap[current.substr(current.find(' ')+1)];
            }
            printEncode(encode);
            j += 2;
          }
        }else if(current.substr(0, current.find(' ')) == "DOTID"){
          string tempStr = this->lines[i][j+1];
          string ident = tempStr.substr(0, tempStr.find(' '));
          int encode;
          if(ident == "DECINT"){
            encode = stoll(tempStr.substr(tempStr.find(' ')+1));
          }else if(ident == "HEXINT"){
            encode = stoul(tempStr.substr(tempStr.find(' ')+1), nullptr, 16);
          }else{
            encode = symbol_map[tempStr.substr(tempStr.find(' ')+1)];
          }
          printEncode(encode);
          j += 2;
        }else{
          throw runtime_error("Invalid Opcode Error: " + current + "\n");
        }

      }
    }
  }

  void printEncode(int encode){
    string str;
    for (int i = 31; i >= 0; --i) {
      str += ((encode & (1 << i)) ? '1' : '0');
    }
    char digit = 0;
    int power = 7;
    for(size_t i = 0; i < str.size(); i++){
      char c = str[i];
      if(c == '1'){
          digit += pow(2, power);
      }
      power--;
      if(power < 0){
          cout << digit;
          digit = 0;
          power = 7;
      }
    }
  }

};

//// Function that takes a DFA file (passed as a stream) and prints information about it.
void DFAprint(std::istream &in, DFA &dfa);
//// These helper functions are defined at the bottom of the file:
// Check if a string is a single character.
bool isChar(std::string s);
// Check if a string represents a character range.
bool isRange(std::string s);
// Remove leading and trailing whitespace from a string, and
// squish intermediate whitespace sequences down to one space each.
std::string squish(std::string s);
// Convert hex digit character to corresponding number.
int hexToNum(char c);
// Convert number to corresponding hex digit character.
char numToHex(int d);
// Replace all escape sequences in a string with corresponding characters.
std::string escape(std::string s);
// Convert non-printing characters or spaces in a string into escape sequences.
std::string unescape(std::string s);
// my good old simple maximal munch
std::string simple_MM(std::string s, DFA &dfa);
// scan the file
vector<string> scanFile();
// mamaMiya converter
vector<string> mamaMiya(vector<string> input, DFA &dfa);
bool regInRange(const string& num);

// assembler functions

//fill ass with lines
//void fillAss(Assembler &ass, vector<string> list);

const std::string STATES      = ".STATES";
const std::string TRANSITIONS = ".TRANSITIONS";
const std::string INPUT       = ".INPUT";
const std::regex hexPattern("^[0-9A-Fa-f]+$");
const std::regex decPattern("^[-]?[0-9]+$");
const std::regex regPattern("^\\$\\d+$");

int main() {
  DFA dfa;
  Assembler ass;
  try {
    std::stringstream s(DFAstring);
    DFAprint(s, dfa);
    vector<string> list = scanFile();
    // cout << endl;
    // for(size_t i = 0; i < list.size(); i++){
    //   cout << list[i] << endl;
    // }
    vector<string> result = mamaMiya(list, dfa);
    // for(size_t i = 0; i < list.size(); i++){
    //   cout << result[i] << endl;
    // }
    // cout << endl;
    ass.fillAss(result);
    // for(size_t i = 0; i < ass.lines.size(); i++){
    //   cout << "Line " + std::to_string(i) + " is: " << endl;
    //   for(size_t j = 0; j < ass.lines[i].size(); j++){
    //     cout << ass.lines[i][j] + "  ";
    //   }
    //   cout << endl;
    // }
    // cout << endl;
    ass.fillAssTable();
    // for (const auto& pair : ass.symbol_map) {
    //     std::cout << pair.first << ": " << pair.second << std::endl;
    // }
    // for (const auto& pair : ass.IDMap) {
    //     std::cout << pair.first << ": " << pair.second << std::endl;
    // }
    ass.analysisCheck();

    ass.synthesisEncoding();
  } catch(std::runtime_error &e) {
    std::cerr << "ERROR: " << e.what() << "\n";
    return 1;
  }

  return 0;
}

// Scanner Funtions

vector<string> mamaMiya(vector<string> input, DFA &dfa){
  vector<string> result;
  string inputStr;
  string output;
  for(size_t i = 0; i < input.size(); i++){
    inputStr = input[i];
    output = simple_MM(inputStr, dfa);
    if(output == "ZERO"){
      output = "DECINT";
    }
    string out = inputStr == "\n" ? output : output + " " + inputStr;
    result.push_back(out);
  }
  return result;
}

vector<string> scanFile(){
  vector<string> list;
  string line;
  while(getline(cin, line)){
    istringstream iss(line);
    char ch;
    string temp = "";
    for (size_t i = 0; i < line.length(); ++i) {
      line.erase(0, line.find_first_not_of(' '));
      ch = line[i];

      if(ch == ';'){
        break;
      }

      if (ch == ' ' || ch == '(' || ch == ')' || ch == '\n' || ch == ',' || ch == '.'  || ch == ':' || ch == '$') {
          if(ch == ':'){
            temp += ch;
          }
          if (!temp.empty()) {
              list.push_back(temp);
              temp.clear();
          }
          if(ch == '$' || ch == '.'){
            temp += ch;
          }
          if (ch != ' ' && ch != ':' && ch != '$' && ch != '.') {
              list.push_back(string(1, ch));
          }
      } else {
          temp += ch;
      }

      if(temp.size() > 1 && temp[0] == '$'){
        if(regex_match(temp.substr(0,temp.length()-1), regPattern) && !regex_match(temp, regPattern)){
          list.push_back(temp.substr(0,temp.length()-1));
          temp = temp.substr(temp.length()-1);
        }
      }else if(temp.size() > 2 && temp.substr(0,2) == "0x"){
        if(!regex_match(temp.substr(2), hexPattern)){
          list.push_back(temp.substr(0,temp.length()-1));
          temp = temp.substr(temp.length()-1);
        }
      }else if(!isdigit(ch)){
        if(regex_match(temp.substr(0,temp.length()-1), decPattern) && temp != "0x"){
          list.push_back(temp.substr(0,temp.length()-1));
          temp = temp.substr(temp.length()-1);
        }
      }

    }

    if (!temp.empty()) { 
      list.push_back(temp);
    }

    // if(line.size() != 0){
      list.push_back("\n");
    // }
    
    
  }
  return list;
}

string simple_MM(string s, DFA& dfa){
  string fromState = "start";
  string nextState;
  for (size_t i = 0; i < s.length(); ++i) {
    char c = s[i];
    nextState = dfa.getNextState(fromState, c);
    if(nextState == "8964"){
      throw runtime_error("Invalid State: " + s + "\n");
    }
    fromState = nextState;
  }
  // handle your mother's errors
  if(nextState == "REGISTER" && !regInRange(s)){
    throw runtime_error("Invalid Register Number: " + s + "\n");
  }
  if(!dfa.dfaStates[nextState]){
    throw runtime_error("End in Not Accepting State: " + nextState + "\n");
  }
  if(nextState == "DECINT" && !isWithinRange(s, "-2147483648", "4294967295")){
    throw runtime_error("Decint not in range: " + s + "\n");
  }
  if(nextState == "HEXINT" && !hexIsWithinRange(s, "0xFFFFFFFF")){
    throw runtime_error("Hexint not in range: " + s + "\n");
  }
  return nextState;
}

bool regInRange(const string& num){
  size_t startPos = 1;
  while(startPos < num.length() && num[startPos] == '0'){
    startPos++;
  }
  if(startPos == num.length()) {
    return true;
  }
  string newNum = num.substr(startPos);
  if(newNum.size() > 2 || atoi(newNum.c_str()) > 31){
    return false;
  }
  return true;
}

bool isWithinRange(const std::string& num, string low_bound, string up_bound){
  const string max_p = up_bound;
  const string max_n = low_bound;
  size_t startPos = num[0] == '-' ? 1 : 0;
  while(startPos < num.length() && num[startPos] == '0') {
    startPos++;
  }
  if(startPos == num.length()) {
    return true;
  }
  string newNum;
  if(num[0] == '-'){
    newNum = "-" + num.substr(startPos);
  }else{
    newNum = num.substr(startPos);
  }
  if (newNum[0] == '-') {
      if (newNum.length() > max_n.length()) {
          return false;
      }
      if (newNum.length() == max_n.length() && newNum > max_n) {
          return false;
      }
  } else {
      if (newNum.length() > max_p.length()) {
          return false;
      }
      if (newNum.length() == max_p.length() && newNum > max_p) {
          return false;
      }
  }
  return true;
}

bool hexIsWithinRange(const string& num, string bound){  
  if(num.length() <= 2 || (num.length() > 2 && num.substr(0,2) != "0x")){
    return false;
  }
  size_t startPos = 2;
  while(startPos < num.length() && num[startPos] == '0') {
    startPos++;
  }
  if(startPos == num.length()) {
    return true;
  }
  string newNum = "0x" + num.substr(startPos);
  if(newNum.length() > 10){
    return false;
  }
  return stoul(newNum, nullptr, 16) <= stoul(bound, nullptr, 16);
}

void DFAprint(std::istream &in, DFA &dfa) {
  std::string s;
  // Skip blank lines at the start of the file
  while(true) {
    if (!(std::getline(in, s))) {
      throw std::runtime_error
        ("Expected " + STATES + ", but found end of input.");
    }
    s = squish(s);
    if (s == STATES) {
      break;
    }
    if (!s.empty()) {
      throw std::runtime_error
        ("Expected " + STATES + ", but found: " + s);
    }
  }
  // Print states
  //std::cout << "States:" << '\n';
  bool initial = true;
  while(true) {
    if (!(in >> s)) {
      throw std::runtime_error
        ("Unexpected end of input while reading state set: " 
         + TRANSITIONS + "not found.");
    }
    if (s == TRANSITIONS) {
      break;
    } 
    // Process an individual state
    bool accepting = false;
    if (s.back() == '!' && s.length() > 1) {
      accepting = true;
      s.pop_back();
    }
    // std::cout << s 
    //           << (initial   ? " (initial)"   : "") 
    //           << (accepting ? " (accepting)" : "") << '\n';
    // Only the first state is marked as initial
    if(initial || !accepting){
        dfa.dfaStates[s] = false;
    }else{
        dfa.dfaStates[s] = true;
    }
    initial = false;
  }
  // Print transitions
  //std::cout << "Transitions:" << '\n';
  std::getline(in, s); // Skip .TRANSITIONS header
  while(true) {
    if (!(std::getline(in, s))) {
      // We reached the end of the file
      break;
    }
    s = squish(s);
    if (s == INPUT) {
      break;
    } 
    // Split the line into parts
    std::string lineStr = s;
    std::stringstream line(lineStr);
    std::vector<std::string> lineVec;
    while(line >> s) {
      lineVec.push_back(s);
    }
    if(lineVec.empty()) {
      // Skip blank lines
      continue;
    }
    if (lineVec.size() < 3) {
      throw std::runtime_error
        ("Incomplete transition line: " + lineStr);
    }
    // Extract state information from the line
    std::string fromState = lineVec.front();
    std::string toState = lineVec.back();
    // Extract character and range information from the line
    std::vector<char> charVec;
    for(size_t i = 1; i < lineVec.size()-1; ++i) {
      std::string charOrRange = escape(lineVec[i]);
      if (isChar(charOrRange)) {
        char c = charOrRange[0];
        if (c < 0 || c > 127) {
          throw std::runtime_error
            ("Invalid (non-ASCII) character in transition line: " + lineStr + "\n"
             + "Character " + unescape(std::string(1,c)) + " is outside ASCII range");
        }
        charVec.push_back(c);
      } else if (isRange(charOrRange)) {
        for(char c = charOrRange[0]; charOrRange[0] <= c && c <= charOrRange[2]; ++c) {
          charVec.push_back(c);
        }
      } else {
        throw std::runtime_error
          ("Expected character or range, but found "
           + charOrRange + " in transition line: " + lineStr);
      }
    }
    // Print a representation of the transition line
    //std::cout << fromState << ' ';
    for ( char c : charVec ) {
      string res = unescape(std::string(1,c));
      //string stateRes = fromState + "_" + res;
      //std::cout << res << ' ';
      dfa.nextStates[{fromState, c}] = toState;
    }
    //std::cout << toState << '\n';
  }
  // We ignore .INPUT sections, so we're done
}

//// Helper functions

bool isChar(std::string s) {
  return s.length() == 1;
}

bool isRange(std::string s) {
  return s.length() == 3 && s[1] == '-';
}

std::string squish(std::string s) {
  std::stringstream ss(s);
  std::string token;
  std::string result;
  std::string space = "";
  while(ss >> token) {
    result += space;
    result += token;
    space = " ";
  }
  return result;
}

int hexToNum(char c) {
  if ('0' <= c && c <= '9') {
    return c - '0';
  } else if ('a' <= c && c <= 'f') {
    return 10 + (c - 'a');
  } else if ('A' <= c && c <= 'F') {
    return 10 + (c - 'A');
  }
  // This should never happen....
  throw std::runtime_error("Invalid hex digit!");
}

char numToHex(int d) {
  return (d < 10 ? d + '0' : d - 10 + 'A');
}

std::string escape(std::string s) {
  std::string p;
  for(size_t i=0; i<s.length(); ++i) {
    if (s[i] == '\\' && i+1 < s.length()) {
      char c = s[i+1]; 
      i = i+1;
      if (c == 's') {
        p += ' ';            
      } else
      if (c == 'n') {
        p += '\n';            
      } else
      if (c == 'r') {
        p += '\r';            
      } else
      if (c == 't') {
        p += '\t';            
      } else
      if (c == 'x') {
        if(i+2 < s.length() && isxdigit(s[i+1]) && isxdigit(s[i+2])) {
          if (hexToNum(s[i+1]) > 8) {
            throw std::runtime_error(
                "Invalid escape sequence \\x"
                + std::string(1, s[i+1])
                + std::string(1, s[i+2])
                +": not in ASCII range (0x00 to 0x7F)");
          }
          char code = hexToNum(s[i+1])*16 + hexToNum(s[i+2]);
          p += code;
          i = i+2;
        } else {
          p += c;
        }
      } else
      if (isgraph(c)) {
        p += c;            
      } else {
        p += s[i];
      }
    } else {
       p += s[i];
    }
  }  
  return p;
}

std::string unescape(std::string s) {
  std::string p;
  for(size_t i=0; i<s.length(); ++i) {
    char c = s[i];
    if (c == ' ') {
      p += "\\s";
    } else
    if (c == '\n') {
      p += "\\n";
    } else
    if (c == '\r') {
      p += "\\r";
    } else
    if (c == '\t') {
      p += "\\t";
    } else
    if (!isgraph(c)) {
      std::string hex = "\\x";
      p += hex + numToHex((unsigned char)c/16) + numToHex((unsigned char)c%16);
    } else {
      p += c;
    }
  }
  return p;
}

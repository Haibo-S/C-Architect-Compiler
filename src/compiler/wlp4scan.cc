#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <unordered_map>
#include <cstdlib>
#include <regex>
#include <cctype>
using namespace std;

//// The dfa.h header defines a string constant containing a DFA file:
// std::string DFAstring = ...
#include "dfa2.h"
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
// bool decisinrange checks if the input is in range
bool isWithinRange(const std::string& num);
// bool hexisinrange checks is the input is in range
bool hexIsWithinRange(const std::string& num);
// bool reg is in range
bool regInRange(const string& num);

const std::string STATES      = ".STATES";
const std::string TRANSITIONS = ".TRANSITIONS";
const std::string INPUT       = ".INPUT";
const std::regex hexPattern("^[0-9A-Fa-f]+$");
const std::regex decPattern("^[-]?[0-9]+$");
const std::regex regPattern("^\\$\\d+$");

int main() {
  DFA dfa;
  try {
    std::stringstream s(DFAstring);
    DFAprint(s, dfa);
    // for(const auto& entry : dfa.nextStates){
    //     cout << "State: " << entry.first.first << ", Input: " << entry.first.second;
    //     cout << " -> Next State: " << entry.second << endl;
    // }
    vector<string> list = scanFile();
    // cout << endl;
    // for(size_t i = 0; i < list.size(); i++){
    //   cout << list[i] << endl;
    // }
    vector<string> result = mamaMiya(list, dfa);
    for(size_t i = 0; i < list.size(); i++){
      cout << result[i] << endl;
    }

  } catch(std::runtime_error &e) {
    std::cerr << "ERROR: " << e.what() << "\n";
    return 1;
  }

  //cout << endl;
  // cout << dfa.getNextState("start", '!') << endl;
  // cout << dfa.getNextState("NEND", '=') << endl;
  // cout << dfa.getNextState("ID2", 't') << endl;
  //  cout << simple_MM("!=", dfa) << endl;
  // cout << hexIsWithinRange("0x0d00") << endl;
  return 0;
}

vector<string> mamaMiya(vector<string> input, DFA &dfa){
  vector<string> result;
  string inputStr;
  string output;
  for(size_t i = 0; i < input.size(); i++){
    inputStr = input[i];
    output = simple_MM(inputStr, dfa);
    if(output.substr(0,2) == "ID"){
      output = "ID";
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

      if(ch == '/' && i + 1 < line.length() && line[i+1] == '/'){
        break;
      }

      if (ch == ' ' || ch == '('  || ch == ')' || ch == '{'  || ch == '}' || ch == '[' || ch == ']' || ch == '=' || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || ch == '&' || ch == ',' || ch == ';' || ch == '<' || ch == '>' || ch == '=' || ch == '!') {
        if (!temp.empty()) {
          list.push_back(temp);
          temp.clear();
        }
        if(ch == '<' && i + 1 < line.length() && line[i+1] == '='){
          temp += ch;
          temp += line[i+1];
          list.push_back(temp);
          temp.clear();
          i += 1;
        }else if(ch == '>' && i + 1 < line.length() && line[i+1] == '='){
          temp += ch;
          temp += line[i+1];
          list.push_back(temp);
          temp.clear();
          i += 1;
        }else if(ch == '=' && i + 1 < line.length() && line[i+1] == '='){
          temp += ch;
          temp += line[i+1];
          list.push_back(temp);
          temp.clear();
          i += 1;
        }else if(ch == '!' && i + 1 < line.length() && line[i+1] == '='){
          temp += ch;
          temp += line[i+1];
          list.push_back(temp);
          temp.clear();
          i += 1;
        }else if(ch != ' '){
          list.push_back(string(1, ch));
        }
      } else {
        temp += ch;
      }

      if(isdigit(ch) && i + 1 < line.length() && !isdigit(line[i+1])){
        if(isdigit(temp[0])){
          list.push_back(temp);
          temp.clear();
        }
      }

    }

    if (!temp.empty()) { 
      list.push_back(temp);
    }

    // if(line.size() != 0){
    //  list.push_back("\n");
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
  // if(nextState == "REGISTER" && !regInRange(s)){
  //   throw runtime_error("Invalid Register Number: " + s + "\n");
  // }
  if(!dfa.dfaStates[nextState]){
    throw runtime_error("End in Not Accepting State: " + nextState + "\n");
  }
  if(nextState == "NUM" && !isWithinRange(s)){
    throw runtime_error("NUM not in range: " + s + "\n");
  }
  if(nextState == "NUM" && s.length() >= 2 && s[0] == '0'){
    throw runtime_error("Too much leading zero: " + s + "\n");
  }
  // if(nextState == "HEXINT" && !hexIsWithinRange(s)){
  //   throw runtime_error("Hexint not in range: " + s + "\n");
  // }
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

bool isWithinRange(const std::string& num){
  const string max_p = "2147483647";
  if (num.length() > max_p.length()) {
      return false;
  }
  if (num.length() == max_p.length() && num > max_p) {
      return false;
  }
  return true;
}

bool hexIsWithinRange(const string& num){  
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
  return stoul(newNum, nullptr, 16) <= stoul("0xFFFFFFFF", nullptr, 16);
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

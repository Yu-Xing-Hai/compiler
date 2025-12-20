#pragma once

#include <unordered_set>
#include <string>
using namespace std;

enum TokenType {
    KEYWORD = 0,      // 0
    IDENTIFIER = 1,   // 1
    INTEGER = 2,      // 2
    FLOAT = 3,        // 3
    CHARACTER = 4,    // 4
    STRING = 5,       // 5
    BOOLEAN = 6,      // 6
    OPERATOR = 7,     // 7
    DELIMITER = 8,    // 8
    COMMENT = 9,      // 9
    UNKNOWN = 10,     // 10
    ERROR = 11        // 11
};

// 定义Token结构
struct Token {
    TokenType type;
    string value;
    int lineNum;
    int colNum;
};

extern int lineNum;
extern int colNum;

void lexicalAnalysis(string code, ofstream& outFile);

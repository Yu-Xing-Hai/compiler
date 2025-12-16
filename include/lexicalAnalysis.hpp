#pragma once

#include <unordered_set>
#include <string>
using namespace std;

enum TokenType {
    KEYWORD,  // 关键字
    IDENTIFIER,  // 标识符
    INTEGER,  // 整数
    FLOAT,  // 浮点数
    CHARACTER,  // 字符
    STRING,  // 字符串
    BOOLEAN,  // 布尔值
    OPERATOR,  // 运算符
    DELIMITER,  // 分隔符
    COMMENT,  // 注释(当前只支持单行注释)
    UNKNOWN,  // 未知
    ERROR,  // 错误
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

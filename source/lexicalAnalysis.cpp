#include "lexicalAnalysis.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>

using namespace std;

// 建立关键字表
unordered_set<string> keyWord = {
    // 声明类
    "int",
    "double",
    "char",
    "bool",
    "void",
    "float",
    // 控制流
    "if",
    "else",
    "while",
    "for",
    "do",
    "break",
    "continue",
    "return",
    // 其他
    "const",
    "struct",
    "enum",
    "typedef",
    "static",
    "extern",
    "auto",
    "virtual",
    "template",
    "class",
    "namespace",
    "using",
    "switch",
    "case",
};

// 定义字母、数字、特殊符号表
unordered_set<int> number = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

unordered_set<char> word = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '_' // 下划线作为特殊字符存放在这里
};

void outPutToken(Token &token, TokenType &tokenType, string &words,
                 ofstream &outFile) {
  if (tokenType == IDENTIFIER) {
    if (keyWord.find(words) != keyWord.end()) {
      tokenType = KEYWORD;
    }
    // 关键字/标识符结束，输出token
    token.type = tokenType;
    token.value = words;
    // 文件输出token
    outFile << "{ " << token.type << " " << token.value << " " << token.lineNum
            << " " << token.colNum << " }" << endl;
    words = "";
    tokenType = UNKNOWN;
  } else if (tokenType == INTEGER) {
    token.type = tokenType;
    token.value = words;

    outFile << "{ " << token.type << " " << token.value << " " << token.lineNum
            << " " << token.colNum << " }" << endl;
    words = "";
    tokenType = UNKNOWN;
  } else if (tokenType == FLOAT) {
    token.type = tokenType;
    token.value = words;

    outFile << "{ " << token.type << " " << token.value << " " << token.lineNum
            << " " << token.colNum << " }" << endl;
    words = "";
    tokenType = UNKNOWN;
  } else if (tokenType == DELIMITER) {
    // 定义结束，输出token
    token.type = tokenType;
    token.value = words;
    // 文件输出token
    outFile << "{ " << token.type << " " << token.value << " " << token.lineNum
            << " " << token.colNum << " }" << endl;
    words = "";
    tokenType = UNKNOWN;
  } else if (tokenType == OPERATOR) {
    // 运算符结束，输出token
    token.type = tokenType;
    token.value = words;
    // 文件输出token
    outFile << "{ " << token.type << " " << token.value << " " << token.lineNum
            << " " << token.colNum << " }" << endl;
    words = "";
    tokenType = UNKNOWN;
  } else if (tokenType == CHARACTER) {
    // 字符结束，输出token
    token.type = tokenType;
    token.value = words;
    // 文件输出token
    outFile << "{ " << token.type << " " << token.value << " " << token.lineNum
            << " " << token.colNum << " }" << endl;
    words = "";
    tokenType = UNKNOWN;
  } else if (tokenType == STRING) {
    // 字符串结束，输出token
    token.type = tokenType;
    token.value = words;
    // 文件输出token
    outFile << "{ " << token.type << " " << token.value << " " << token.lineNum
            << " " << token.colNum << " }" << endl;
    words = "";
    tokenType = UNKNOWN;
  } else if (tokenType == COMMENT) {
    // 注释结束，输出token
    token.type = tokenType;
    token.value = words;
    // 文件输出token
    outFile << "{ " << token.type << " " << token.value << " " << token.lineNum
            << " " << token.colNum << " }" << endl;
    words = "";
    tokenType = UNKNOWN;
  }
}

void outPutError(Token &token, TokenType &tokenType, string &words) {
  cout << "Error: Invalid token at lineNum " << token.lineNum << ", colNumumn "
       << token.colNum << endl;
  cout << "{ " << token.type << " " << token.value << " " << token.lineNum
       << " " << token.colNum << " }" << endl;
  words = "";
  tokenType = UNKNOWN;
}

void lexicalAnalysis(string code, ofstream &outFile) {
  int stringSize = code.size();

  string words = "";
  TokenType tokenType = UNKNOWN;
  Token token = {UNKNOWN, "", lineNum, colNum};
  for (int i = 0; i < stringSize; i++) {
    char scanWord = code[i];
    if (scanWord == ' ') { // 特殊字符
      if (tokenType == IDENTIFIER || tokenType == INTEGER || tokenType == FLOAT) {
        token.lineNum = lineNum;
        token.colNum = colNum - words.size();
        outPutToken(token, tokenType, words, outFile);
      } else if (tokenType == CHARACTER || tokenType == STRING) {
        words += ' ';
      } else if (tokenType == COMMENT) {
        words += scanWord;
        colNum++;
      } else if (tokenType == ERROR) {
        token.lineNum = lineNum;
        token.colNum = colNum - words.size();
        outPutError(token, tokenType, words);
      }
      colNum++;
    } else if (scanWord == '\t') { // 特殊字符
      colNum += 4;
      if (tokenType == STRING) {
        words += scanWord;
      }
    } else if (scanWord == ';') { // 特殊字符
      if (tokenType == STRING || tokenType == CHARACTER) {
        words += scanWord;
        colNum++;
        continue;
      }
      if (words.size() > 0) {
        token.lineNum = lineNum;
        token.colNum = colNum - words.size();
        outPutToken(token, tokenType, words, outFile);
      }
      words += ";";
      tokenType = DELIMITER;
      token.lineNum = lineNum;
      token.colNum = colNum;
      outPutToken(token, tokenType, words, outFile);
      colNum++;
    } else if (word.find(scanWord) != word.end() || scanWord == '_') {
      if (tokenType == UNKNOWN) {
        tokenType = IDENTIFIER;
      }
      if (tokenType == IDENTIFIER || tokenType == CHARACTER ||
          tokenType == STRING || tokenType == COMMENT) {
        if (tokenType == IDENTIFIER) {
          words += scanWord;
          colNum++;
        } else if (tokenType == CHARACTER || tokenType == STRING ||
                   tokenType == COMMENT) {
          words += scanWord;
          colNum++;
        }
      } else if (tokenType == COMMENT) {
        words += scanWord;
        colNum++;
      } else { // 如INTEGER类型时遇见字母
        tokenType = ERROR;
        words += scanWord;
        colNum++;
      }
    } else if (number.find(scanWord) != number.end()) {
      words += scanWord;
      colNum++;
      if (tokenType == UNKNOWN) {
        tokenType = INTEGER;
      }
    } else if (scanWord == '.') {
      if(tokenType == UNKNOWN) {
        tokenType = FLOAT;
      }else if (tokenType == INTEGER) {
        tokenType = FLOAT;
      } else if (tokenType == FLOAT) {
        tokenType = ERROR;
      }
      words += scanWord;
      colNum++;
    } else if (scanWord == '=' || scanWord == '+' || scanWord == '-' ||
               scanWord == '*' || scanWord == '(' ||
               scanWord == ')') {
      if (tokenType == STRING || tokenType == CHARACTER) {
        words += scanWord;
        colNum++;
        continue;
      }
      if (words.size() > 0) {
        token.lineNum = lineNum;
        token.colNum = colNum - words.size();
        outPutToken(token, tokenType, words, outFile);
      }
      words += scanWord;
      tokenType = OPERATOR;
      token.lineNum = lineNum;
      token.colNum = colNum;
      outPutToken(token, tokenType, words, outFile);
      colNum++;
    } else if (scanWord == '"') {
      words += scanWord;
      colNum++;
      if (tokenType == UNKNOWN) {
        tokenType = STRING;
      } else if (tokenType == STRING) {
        token.lineNum = lineNum;
        token.colNum = colNum - words.size();
        outPutToken(token, tokenType, words, outFile);
      }
    } else if (scanWord == '\'') {
      words += scanWord;
      colNum++;
      if (tokenType == UNKNOWN) {
        tokenType = CHARACTER;
      } else if (tokenType == CHARACTER) {
        token.lineNum = lineNum;
        token.colNum = colNum - words.size();
        outPutToken(token, tokenType, words, outFile);
      }
    } else if (scanWord == '/') {
      words += scanWord;
      colNum++;
      if (tokenType == UNKNOWN) {
        tokenType = COMMENT;
      }
    } else {
      words += scanWord;
      colNum++;
    }
  }
  if (tokenType == COMMENT) {
    token.lineNum = lineNum;
    token.colNum = colNum;
    outPutToken(token, tokenType, words, outFile);
  }
}
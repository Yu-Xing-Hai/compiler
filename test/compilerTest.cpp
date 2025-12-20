#include "grammaticalAnalysis.hpp"
#include "lexicalAnalysis.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

int lineNum = 0;
int colNum = 1;

int main() {
  cout << "===========Lexical Analysis=============" << endl;
  // 正确输入测试用例路径：../build/program.cpp
  // 错误输入测试用例路径：../build/errorProgram.cpp
  string inputFilePath = "../build/program.cpp";
  string outputFilePath = "../build/tokens.txt";

  ifstream inFile(inputFilePath);
  if (!inFile.is_open()) {
    cerr << "Error: Failed to open input file " << inputFilePath << "！" << endl;
    cerr << "Please check: 1. File path is correct 2. File exists 3. Read permission"
         << endl;
    return 1;
  }

  ofstream outFile(outputFilePath);
  if (!outFile.is_open()) {
    cerr << "Error: Failed to open tokens.txt" << endl;
    return 1;
  }

  string line = ""; // 存储当前行的内容

  outFile << "===========Tokens Structure=============" << endl;
  outFile << "{type value line col}" << endl;

  while (getline(inFile, line)) {
    lineNum++;
    colNum = 1;
    lexicalAnalysis(line, outFile);
  }
  // 刷新缓冲区，确保所有数据写入文件
  outFile.flush();

  outFile.close();
  cout << "\nTokens has been successfully output to ../build/tokens.txt"
       << endl;
  cout << '\n';

  // 将token.txt中的token转换为vector<string>
  string tokensFilePath = "../build/tokens.txt";
  ifstream tokensFile(tokensFilePath);
  if (!tokensFile.is_open()) {
    cerr << "Error: Failed to open tokens file " << tokensFilePath << "！" << endl;
    cerr << "Please check: 1. File path is correct 2. File exists 3. Read permission"
         << endl;
    return 1;
  }
  string tokenLine = ""; // 存储读出的token
  vector<string> inputBelt;
  while (getline(tokensFile, tokenLine)) {
    if (tokenLine.size() < 4)
      continue;
    // 1. 剔除token首尾的{}，只保留中间内容
    string content =
        tokenLine.substr(2, tokenLine.size() - 4); // 跳过"{ "，截断最后的" }"

    // 2. 拆分内容提取type和value
    istringstream iss(content);
    int type;
    string value;
    if (!(iss >> type >> value))
      continue; // 先读type(7)，再读value(=)，后面的行/列直接忽略,
                // 解析失败直接跳过
    // 4. 映射成语法终结符（严格对齐VT）
    switch (type) {
    case KEYWORD:
      inputBelt.push_back(value);
      break;
    case IDENTIFIER:
      inputBelt.push_back("IDENTIFIER");
      break;
    case INTEGER:
      inputBelt.push_back("INTEGER");
      break;
    case FLOAT:
      inputBelt.push_back("FLOAT");
      break;
    case CHARACTER:
      inputBelt.push_back("CHAR");
      break; // 修正：CHAR而非CHARACTER
    case STRING:
      inputBelt.push_back("STRING");
      break;
    case BOOLEAN:
      inputBelt.push_back(value);
      break;
    case OPERATOR:
      inputBelt.push_back(value);
      break;
    case DELIMITER:
      inputBelt.push_back(value);
      break;
    case COMMENT:
      continue; // 跳过注释
    case UNKNOWN:
      cerr << "Warning: Unknown Token type(" << type << ")，has been skipped" << endl;
      continue;
    case ERROR:
      cerr << "Error: Lexical error Token(" << type << ")，terminate analysis" << endl;
      return 1;
    default:
      cerr << "Error: Invalid Token type " << type << endl;
      return 1;
    }
  }
  inputBelt.push_back("#");

  // 调试用：打印生成的输入流，验证是否正确
  cout << "Generated grammatical input stream:\n";
  for (const auto &s : inputBelt) {
    cout << s << " ";
  }
  cout << '\n';

  cout << "===========Grammatical Analysis=============" << endl;
  GrammaticalAnalysis(inputBelt);
  tokensFile.close();
  return 0;
}
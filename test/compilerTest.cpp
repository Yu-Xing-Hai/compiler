#include "lexicalAnalysis.hpp"
#include "grammaticalAnalysis.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

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
    cerr << "错误：无法打开输入文件 " << inputFilePath << "！" << endl;
    cerr << "请检查：1.文件路径是否正确 2.文件是否存在 3.是否有读取权限"
         << endl;
    return 1;
  }

  ofstream outFile(outputFilePath);
  if (!outFile.is_open()) {
    cerr << "Error: Failed to open tokens.txt" << endl;
    return 1;
  }

  string line = "";  // 存储当前行的内容

  outFile << "===========Tokens Structure=============" << endl;
  outFile << "{type value line col}" << endl;

  while(getline(inFile, line)) {
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

  cout << "===========Grammatical Analysis=============" << endl;
  GrammaticalAnalysis("abccd#");
  return 0;
}
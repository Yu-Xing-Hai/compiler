#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

void GrammaticalAnalysis(std::string inputBelt) {
  // 初始化下推符号栈
  stack<string> symbolStack;
  symbolStack.push("#"); // 栈底
  symbolStack.push("S"); // 开始符

  // 初始化相关数据结构
  vector<string> outputBelt; // 初始化输出带，用于保存使用过的文法规则(左分析)
  unordered_set<string> VN = {
      // 非终结符集合
      "S",
      "A",
  };
  unordered_set<string> VT = {
      // 终结符集合
      "a",
      "b",
      "c",
      "d",
  };
  string endSymbol = "#"; // 结束符
  // 初始化预测分析表
  map<pair<string, string>, string> predictionTable = {
      {{"S", "a"}, "aS"},
      {{"S", "b"}, "bA"},
      {{"A", "c"}, "ccA"},
      {{"A", "d"}, "d"},
  };

  // 实现总控程序
  string::size_type inputBeltIndex = 0; // 输入带索引【疑惑点】
  bool flag = false;
  inputBelt += "#"; // 输入带末尾添加结束符
  string::size_type inputBeltSize = inputBelt.size();
  while (inputBeltIndex < inputBeltSize) {
    string currentSymbol(1, inputBelt[inputBeltIndex]);
    string stackTop = symbolStack.top();
    symbolStack.pop();

    // 情况一
    if (stackTop == endSymbol) {
      if (currentSymbol == endSymbol) { // 输入也到#，才是真成功
        flag = true;
        break;
      } else {
        cerr << "Analysis Failed: 栈顶是#，但输入未到结束符" << endl;
        flag = false;
        break;
      }
    }
    // 情况二
    else if (VN.find(stackTop) != VN.end()) {
      pair<string, string> temp = {stackTop, currentSymbol};
      if (predictionTable.find(temp) != predictionTable.end()) {
        string production = predictionTable[temp];
        if (production != "ε") { // 空产生式不压栈
          for (string::size_type i = production.size() - 1; i != string::npos; i--) {
    symbolStack.push(string(1, production[i]));
}
        }
        outputBelt.push_back(stackTop + " --> " + production);
      } else {
        // 分析失败
        cerr << "Analysis Failed, the current stack top symbol is: " << stackTop
             << "，current input symbol is: " << currentSymbol << endl;
        flag = false;
        break;
      }
    }
    // 情况三
    else if (VT.find(stackTop) != VT.end()) {
      if (stackTop == currentSymbol) {
        inputBeltIndex++;
      } else {
        // 分析失败
        cerr << "Analysis Failed, the current stack top symbol is: " << stackTop
             << "，current input symbol is: " << currentSymbol << endl;
        flag = false;
        break;
      }
    }
  }

  // 分析成功
  if (flag) {
    cout << "Analysis Successfully, the used production rules are:" << endl;
    for (string production : outputBelt) {
      cout << production << endl;
    }
  }
  // 分析失败
  else {
    cout << "Analysis Failed" << endl;
  }
  return;
}
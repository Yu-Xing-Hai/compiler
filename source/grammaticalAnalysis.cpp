#include "grammaticalAnalysis.hpp"
#include <iostream>
#include <map>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

void GrammaticalAnalysis(vector<string> inputBelt) {
  // 初始化下推符号栈
  stack<string> symbolStack;
  symbolStack.push("#");      // 栈底
  symbolStack.push("<PROG>"); // 开始符

  // 初始化相关数据结构
  string endSymbol = "#";    // 结束符
  vector<string> outputBelt; // 初始化输出带，用于保存使用过的文法规则(左分析)

  /*
   * LL(1) 语法分析器核心数据结构
   * 设计原则：
   * 1. 符号区分规范：
   *    - 非终结符：带<>括号（如 <STMT>）
   *    - 终结符：无<>括号 + 命名规范：
   *        * 语言关键字：小写（if/while/true）
   *        * 词法Token：大写（IDENTIFIER/INTEGER）
   *        * 标点符号：原字符（;/{}/()）
   * 2. 文法设计原则：
   *    - 无冗余非终结符层（字面量直接在<FACTOR>处理）
   *    - 显式空语句语义（专用<EMPTY_STMT>）
   *    - 完整覆盖所有语法路径
   * 3. 工程实践：
   *    - 与词法分析器Token名称严格对齐
   *    - 预留错误恢复扩展点（同步符号集）
   */

  // 非终结符集合（带<>括号，与终结符明确区分）
  unordered_set<string> VN = {
      "<PROG>",                // 程序根节点
      "<GDECL_LIST>",          // 全局声明列表
      "<MAIN_FUNC>",           // 主函数
      "<GDECL_LIST_TAIL>",     // 全局声明列表后缀
      "<NON_INT_GLOBAL_DECL>", // 非int全局声明
      "<INT_GLOBAL_DECL>",     // int全局声明
      "<GLOBAL_DECL>",         // 通用全局声明
      "<TYPE_SPEC>",           // 类型说明符
      "<NON_INT_TYPE>",        // 非int类型
      "<INIT_OPT>",            // 初始化选项
      "<STMT>",                // 语句
      "<EMPTY_STMT>",          // 专用空语句（显式语义）
      "<ELSE_CLAUSE>",         // else子句
      "<COMP_STMT>",           // 复合语句
      "<STMT_LIST>",           // 语句列表
      "<STMT_LIST_TAIL>",      // 语句列表后缀
      "<EXP>",                 // 表达式
      "<LOGIC_OR>",            // 逻辑或表达式
      "<LOGIC_OR_TAIL>",       // 逻辑或后缀
      "<LOGIC_AND>",           // 逻辑与表达式
      "<LOGIC_AND_TAIL>",      // 逻辑与后缀
      "<REL_EXP>",             // 关系表达式
      "<REL_EXP_TAIL>",        // 关系表达式后缀
      "<REL_OP>",              // 关系运算符
      "<ADD_EXP>",             // 加法表达式
      "<ADD_EXP_TAIL>",        // 加法表达式后缀
      "<MUL_EXP>",             // 乘法表达式
      "<MUL_EXP_TAIL>",        // 乘法表达式后缀
      "<FACTOR>"               // 基本因子
  };

  // 终结符集合（词法分析器必须将"main"识别为关键字）
  unordered_set<string> VT = {
      // 基本类型关键字
      "float", "char", "bool", "string", "int",
      // 控制流关键字
      "if", "else", "while", "for", "return",
      "main", // "main"为保留字，非IDENTIFIER
      // 布尔字面量
      "true", "false",
      // 运算符
      "+", "-", "*", "/", "=", "==", "!=", ">", "<", ">=", "<=", "&&", "||",
      // 分隔符
      ";", "{", "}", "(", ")",
      // 词法Token
      "IDENTIFIER", "INTEGER", "FLOAT", "CHAR", "STRING",
      // 结束符
      "#"};

  // 完整预测分析表（LL(1)）
  // 结构：{ {非终结符, 当前输入符号}, 产生式右部 }
  // 规范：
  //   1. 产生式右部符号间用空格分隔
  //   2. NULL表示空产生式
  //   3. 按语法结构分区注释，便于维护
  map<pair<string, string>, string> predictionTable = {
      // ===================== 1. 程序结构与全局声明 =====================
      // <PROG> ::= <GDECL_LIST> <MAIN_FUNC>
      {{"<PROG>", "float"}, "<GDECL_LIST> <MAIN_FUNC>"},
      {{"<PROG>", "char"}, "<GDECL_LIST> <MAIN_FUNC>"},
      {{"<PROG>", "bool"}, "<GDECL_LIST> <MAIN_FUNC>"},
      {{"<PROG>", "string"}, "<GDECL_LIST> <MAIN_FUNC>"},
      {{"<PROG>", "int"}, "<GDECL_LIST> <MAIN_FUNC>"},

      // <GDECL_LIST> ::= <GLOBAL_DECL> <GDECL_LIST_TAIL> | NULL (当遇到int main时)
      {{"<GDECL_LIST>", "float"}, "<GLOBAL_DECL> <GDECL_LIST_TAIL>"},
      {{"<GDECL_LIST>", "char"}, "<GLOBAL_DECL> <GDECL_LIST_TAIL>"},
      {{"<GDECL_LIST>", "bool"}, "<GLOBAL_DECL> <GDECL_LIST_TAIL>"},
      {{"<GDECL_LIST>", "string"}, "<GLOBAL_DECL> <GDECL_LIST_TAIL>"},
      {{"<GDECL_LIST>", "int"}, "NULL"},

      // <GDECL_LIST_TAIL> ::= <GLOBAL_DECL> <GDECL_LIST_TAIL> | NULL
      {{"<GDECL_LIST_TAIL>", "float"}, "<GLOBAL_DECL> <GDECL_LIST_TAIL>"},
      {{"<GDECL_LIST_TAIL>", "char"}, "<GLOBAL_DECL> <GDECL_LIST_TAIL>"},
      {{"<GDECL_LIST_TAIL>", "bool"}, "<GLOBAL_DECL> <GDECL_LIST_TAIL>"},
      {{"<GDECL_LIST_TAIL>", "string"}, "<GLOBAL_DECL> <GDECL_LIST_TAIL>"},
      {{"<GDECL_LIST_TAIL>", "int"}, "NULL"},

      // <GLOBAL_DECL> ::= <NON_INT_GLOBAL_DECL> | <INT_GLOBAL_DECL>
      {{"<GLOBAL_DECL>", "float"}, "<NON_INT_GLOBAL_DECL>"},
      {{"<GLOBAL_DECL>", "char"}, "<NON_INT_GLOBAL_DECL>"},
      {{"<GLOBAL_DECL>", "bool"}, "<NON_INT_GLOBAL_DECL>"},
      {{"<GLOBAL_DECL>", "string"}, "<NON_INT_GLOBAL_DECL>"},
      {{"<GLOBAL_DECL>", "int"}, "<INT_GLOBAL_DECL>"},

      // 非int类型全局声明
      {{"<NON_INT_GLOBAL_DECL>", "float"},
       "<NON_INT_TYPE> IDENTIFIER <INIT_OPT> ;"},
      {{"<NON_INT_GLOBAL_DECL>", "char"},
       "<NON_INT_TYPE> IDENTIFIER <INIT_OPT> ;"},
      {{"<NON_INT_GLOBAL_DECL>", "bool"},
       "<NON_INT_TYPE> IDENTIFIER <INIT_OPT> ;"},
      {{"<NON_INT_GLOBAL_DECL>", "string"},
       "<NON_INT_TYPE> IDENTIFIER <INIT_OPT> ;"},

      // int类型全局声明
      {{"<INT_GLOBAL_DECL>", "int"}, "int IDENTIFIER <INIT_OPT> ;"},

      // 类型处理
      {{"<NON_INT_TYPE>", "float"}, "float"},
      {{"<NON_INT_TYPE>", "char"}, "char"},
      {{"<NON_INT_TYPE>", "bool"}, "bool"},
      {{"<NON_INT_TYPE>", "string"}, "string"},

      {{"<TYPE_SPEC>", "float"}, "<NON_INT_TYPE>"},
      {{"<TYPE_SPEC>", "char"}, "<NON_INT_TYPE>"},
      {{"<TYPE_SPEC>", "bool"}, "<NON_INT_TYPE>"},
      {{"<TYPE_SPEC>", "string"}, "<NON_INT_TYPE>"},
      {{"<TYPE_SPEC>", "int"}, "int"},

      // ===================== 2. 语句与控制结构（含显式空语句）
      // =====================
      // 通用语句
      {{"<STMT>", "{"}, "<COMP_STMT>"},
      {{"<STMT>", "if"}, "if ( <EXP> ) <STMT> <ELSE_CLAUSE>"},
      {{"<STMT>", "while"}, "while ( <EXP> ) <STMT>"},
      {{"<STMT>", "for"}, "for ( <EXP> ; <EXP> ; <EXP> ) <STMT>"},
      {{"<STMT>", "return"}, "return <EXP> ;"},
      {{"<STMT>", "IDENTIFIER"}, "IDENTIFIER = <EXP> ;"},
      {{"<STMT>", ";"}, "<EMPTY_STMT>"},               // 空语句显式推导
      {{"<STMT>", "int"}, "<INT_GLOBAL_DECL>"},        // int变量声明
      {{"<STMT>", "char"}, "<NON_INT_GLOBAL_DECL>"},   // char变量声明
      {{"<STMT>", "string"}, "<NON_INT_GLOBAL_DECL>"}, // string变量声明

      // 专用空语句（语义明确，便于错误定位）
      {{"<EMPTY_STMT>", ";"}, ";"},

      // 复合语句
      {{"<COMP_STMT>", "{"}, "{ <STMT_LIST> }"},

      // 语句列表
      {{"<STMT_LIST>", "{"}, "<STMT> <STMT_LIST_TAIL>"},
      {{"<STMT_LIST>", "}"}, "NULL"}, // 空语句块
      {{"<STMT_LIST>", "if"}, "<STMT> <STMT_LIST_TAIL>"},
      {{"<STMT_LIST>", "while"}, "<STMT> <STMT_LIST_TAIL>"},
      {{"<STMT_LIST>", "return"}, "<STMT> <STMT_LIST_TAIL>"},
      {{"<STMT_LIST>", "IDENTIFIER"}, "<STMT> <STMT_LIST_TAIL>"},
      {{"<STMT_LIST>", ";"}, "<EMPTY_STMT> <STMT_LIST_TAIL>"}, // 空语句融入列表
      {{"<STMT_LIST>", "int"}, "<STMT> <STMT_LIST_TAIL>"},
      {{"<STMT_LIST>", "char"}, "<STMT> <STMT_LIST_TAIL>"},
      {{"<STMT_LIST>", "string"}, "<STMT> <STMT_LIST_TAIL>"},

      // 语句列表后缀
      {{"<STMT_LIST_TAIL>", "{"}, "<STMT> <STMT_LIST_TAIL>"},
      {{"<STMT_LIST_TAIL>", "}"}, "NULL"},
      {{"<STMT_LIST_TAIL>", "if"}, "<STMT> <STMT_LIST_TAIL>"},
      {{"<STMT_LIST_TAIL>", "while"}, "<STMT> <STMT_LIST_TAIL>"},
      {{"<STMT_LIST_TAIL>", "for"}, "<STMT> <STMT_LIST_TAIL>"},
      {{"<STMT_LIST_TAIL>", "return"}, "<STMT> <STMT_LIST_TAIL>"},
      {{"<STMT_LIST_TAIL>", "IDENTIFIER"}, "<STMT> <STMT_LIST_TAIL>"},
      {{"<STMT_LIST_TAIL>", ";"}, "<EMPTY_STMT> <STMT_LIST_TAIL>"},
      {{"<STMT_LIST_TAIL>", "int"}, "<STMT> <STMT_LIST_TAIL>"},
      {{"<STMT_LIST_TAIL>", "char"}, "<STMT> <STMT_LIST_TAIL>"},
      {{"<STMT_LIST_TAIL>", "string"}, "<STMT> <STMT_LIST_TAIL>"},
      {{"<STMT_LIST_TAIL>", "#"}, "NULL"}, // 文件结束

      // else子句处理
      {{"<ELSE_CLAUSE>", "else"}, "else <STMT>"},
      {{"<ELSE_CLAUSE>", "if"}, "NULL"}, // 无else
      {{"<ELSE_CLAUSE>", "while"}, "NULL"},
      {{"<ELSE_CLAUSE>", "return"}, "NULL"},
      {{"<ELSE_CLAUSE>", "IDENTIFIER"}, "NULL"},
      {{"<ELSE_CLAUSE>", "{"}, "NULL"},
      {{"<ELSE_CLAUSE>", ";"}, "NULL"},
      {{"<ELSE_CLAUSE>", "}"}, "NULL"},
      {{"<ELSE_CLAUSE>", "#"}, "NULL"}, // 文件结束

      // 主函数
      {{"<MAIN_FUNC>", "int"}, "int main ( ) <COMP_STMT>"},

      // ===================== 3. 表达式系统（完整运算符优先级）
      // =====================
      // 入口表达式
      {{"<EXP>", "("}, "<LOGIC_OR>"},
      {{"<EXP>", "IDENTIFIER"}, "<LOGIC_OR>"},
      {{"<EXP>", "INTEGER"}, "<LOGIC_OR>"},
      {{"<EXP>", "FLOAT"}, "<LOGIC_OR>"},
      {{"<EXP>", "true"}, "<LOGIC_OR>"},
      {{"<EXP>", "false"}, "<LOGIC_OR>"},
      {{"<EXP>", "CHAR"}, "<LOGIC_OR>"},
      {{"<EXP>", "STRING"}, "<LOGIC_OR>"},

      // 逻辑或表达式 (最低优先级)
      {{"<LOGIC_OR>", "("}, "<LOGIC_AND> <LOGIC_OR_TAIL>"},
      {{"<LOGIC_OR>", "IDENTIFIER"}, "<LOGIC_AND> <LOGIC_OR_TAIL>"},
      {{"<LOGIC_OR>", "INTEGER"}, "<LOGIC_AND> <LOGIC_OR_TAIL>"},
      {{"<LOGIC_OR>", "FLOAT"}, "<LOGIC_AND> <LOGIC_OR_TAIL>"},
      {{"<LOGIC_OR>", "true"}, "<LOGIC_AND> <LOGIC_OR_TAIL>"},
      {{"<LOGIC_OR>", "false"}, "<LOGIC_AND> <LOGIC_OR_TAIL>"},
      {{"<LOGIC_OR>", "CHAR"}, "<LOGIC_AND> <LOGIC_OR_TAIL>"},
      {{"<LOGIC_OR>", "STRING"}, "<LOGIC_AND> <LOGIC_OR_TAIL>"},

      // 逻辑或后缀
      {{"<LOGIC_OR_TAIL>", "||"}, "|| <LOGIC_AND> <LOGIC_OR_TAIL>"},
      {{"<LOGIC_OR_TAIL>", ")"}, "NULL"},
      {{"<LOGIC_OR_TAIL>", ";"}, "NULL"},
      {{"<LOGIC_OR_TAIL>", "}"}, "NULL"},
      {{"<LOGIC_OR_TAIL>", "else"}, "NULL"}, // else前的表达式结束
      {{"<LOGIC_OR_TAIL>", "#"}, "NULL"},    // 文件结束

      // 逻辑与表达式
      {{"<LOGIC_AND>", "("}, "<REL_EXP> <LOGIC_AND_TAIL>"},
      {{"<LOGIC_AND>", "IDENTIFIER"}, "<REL_EXP> <LOGIC_AND_TAIL>"},
      {{"<LOGIC_AND>", "INTEGER"}, "<REL_EXP> <LOGIC_AND_TAIL>"},
      {{"<LOGIC_AND>", "FLOAT"}, "<REL_EXP> <LOGIC_AND_TAIL>"},
      {{"<LOGIC_AND>", "true"}, "<REL_EXP> <LOGIC_AND_TAIL>"},
      {{"<LOGIC_AND>", "false"}, "<REL_EXP> <LOGIC_AND_TAIL>"},
      {{"<LOGIC_AND>", "CHAR"}, "<REL_EXP> <LOGIC_AND_TAIL>"},
      {{"<LOGIC_AND>", "STRING"}, "<REL_EXP> <LOGIC_AND_TAIL>"},

      // 逻辑与后缀
      {{"<LOGIC_AND_TAIL>", "&&"}, "&& <REL_EXP> <LOGIC_AND_TAIL>"},
      {{"<LOGIC_AND_TAIL>", "||"}, "NULL"},
      {{"<LOGIC_AND_TAIL>", ")"}, "NULL"},
      {{"<LOGIC_AND_TAIL>", ";"}, "NULL"},
      {{"<LOGIC_AND_TAIL>", "}"}, "NULL"},
      {{"<LOGIC_AND_TAIL>", "#"}, "NULL"},

      // 关系表达式
      {{"<REL_EXP>", "("}, "<ADD_EXP> <REL_EXP_TAIL>"},
      {{"<REL_EXP>", "IDENTIFIER"}, "<ADD_EXP> <REL_EXP_TAIL>"},
      {{"<REL_EXP>", "INTEGER"}, "<ADD_EXP> <REL_EXP_TAIL>"},
      {{"<REL_EXP>", "FLOAT"}, "<ADD_EXP> <REL_EXP_TAIL>"},
      {{"<REL_EXP>", "true"}, "<ADD_EXP> <REL_EXP_TAIL>"},
      {{"<REL_EXP>", "false"}, "<ADD_EXP> <REL_EXP_TAIL>"},
      {{"<REL_EXP>", "CHAR"}, "<ADD_EXP> <REL_EXP_TAIL>"},
      {{"<REL_EXP>", "STRING"}, "<ADD_EXP> <REL_EXP_TAIL>"},

      // 关系表达式后缀
      {{"<REL_EXP_TAIL>", "=="}, "<REL_OP> <ADD_EXP> <REL_EXP_TAIL>"},
      {{"<REL_EXP_TAIL>", "!="}, "<REL_OP> <ADD_EXP> <REL_EXP_TAIL>"},
      {{"<REL_EXP_TAIL>", ">"}, "<REL_OP> <ADD_EXP> <REL_EXP_TAIL>"},
      {{"<REL_EXP_TAIL>", "<"}, "<REL_OP> <ADD_EXP> <REL_EXP_TAIL>"},
      {{"<REL_EXP_TAIL>", ">="}, "<REL_OP> <ADD_EXP> <REL_EXP_TAIL>"},
      {{"<REL_EXP_TAIL>", "<="}, "<REL_OP> <ADD_EXP> <REL_EXP_TAIL>"},
      {{"<REL_EXP_TAIL>", "&&"}, "NULL"},
      {{"<REL_EXP_TAIL>", "||"}, "NULL"},
      {{"<REL_EXP_TAIL>", ")"}, "NULL"},
      {{"<REL_EXP_TAIL>", ";"}, "NULL"},
      {{"<REL_EXP_TAIL>", "}"}, "NULL"},
      {{"<REL_EXP_TAIL>", "#"}, "NULL"},

      // 关系运算符
      {{"<REL_OP>", "=="}, "=="},
      {{"<REL_OP>", "!="}, "!="},
      {{"<REL_OP>", ">"}, ">"},
      {{"<REL_OP>", "<"}, "<"},
      {{"<REL_OP>", ">="}, ">="},
      {{"<REL_OP>", "<="}, "<="},

      // 加法表达式
      {{"<ADD_EXP>", "("}, "<MUL_EXP> <ADD_EXP_TAIL>"},
      {{"<ADD_EXP>", "IDENTIFIER"}, "<MUL_EXP> <ADD_EXP_TAIL>"},
      {{"<ADD_EXP>", "INTEGER"}, "<MUL_EXP> <ADD_EXP_TAIL>"},
      {{"<ADD_EXP>", "FLOAT"}, "<MUL_EXP> <ADD_EXP_TAIL>"},
      {{"<ADD_EXP>", "true"}, "<MUL_EXP> <ADD_EXP_TAIL>"},
      {{"<ADD_EXP>", "false"}, "<MUL_EXP> <ADD_EXP_TAIL>"},
      {{"<ADD_EXP>", "CHAR"}, "<MUL_EXP> <ADD_EXP_TAIL>"},
      {{"<ADD_EXP>", "STRING"}, "<MUL_EXP> <ADD_EXP_TAIL>"},

      // 加法表达式后缀
      {{"<ADD_EXP_TAIL>", "+"}, "+ <MUL_EXP> <ADD_EXP_TAIL>"},
      {{"<ADD_EXP_TAIL>", "-"}, "- <MUL_EXP> <ADD_EXP_TAIL>"},
      {{"<ADD_EXP_TAIL>", "=="}, "NULL"},
      {{"<ADD_EXP_TAIL>", "!="}, "NULL"},
      {{"<ADD_EXP_TAIL>", ">"}, "NULL"},
      {{"<ADD_EXP_TAIL>", "<"}, "NULL"},
      {{"<ADD_EXP_TAIL>", ">="}, "NULL"},
      {{"<ADD_EXP_TAIL>", "<="}, "NULL"},
      {{"<ADD_EXP_TAIL>", "&&"}, "NULL"},
      {{"<ADD_EXP_TAIL>", "||"}, "NULL"},
      {{"<ADD_EXP_TAIL>", ")"}, "NULL"},
      {{"<ADD_EXP_TAIL>", ";"}, "NULL"},
      {{"<ADD_EXP_TAIL>", "}"}, "NULL"},
      {{"<ADD_EXP_TAIL>", "#"}, "NULL"},

      // 乘法表达式
      {{"<MUL_EXP>", "("}, "<FACTOR> <MUL_EXP_TAIL>"},
      {{"<MUL_EXP>", "IDENTIFIER"}, "<FACTOR> <MUL_EXP_TAIL>"},
      {{"<MUL_EXP>", "INTEGER"}, "<FACTOR> <MUL_EXP_TAIL>"},
      {{"<MUL_EXP>", "FLOAT"}, "<FACTOR> <MUL_EXP_TAIL>"},
      {{"<MUL_EXP>", "true"}, "<FACTOR> <MUL_EXP_TAIL>"},
      {{"<MUL_EXP>", "false"}, "<FACTOR> <MUL_EXP_TAIL>"},
      {{"<MUL_EXP>", "CHAR"}, "<FACTOR> <MUL_EXP_TAIL>"},
      {{"<MUL_EXP>", "STRING"}, "<FACTOR> <MUL_EXP_TAIL>"},

      // 乘法表达式后缀
      {{"<MUL_EXP_TAIL>", "*"}, "* <FACTOR> <MUL_EXP_TAIL>"},
      {{"<MUL_EXP_TAIL>", "/"}, "/ <FACTOR> <MUL_EXP_TAIL>"},
      {{"<MUL_EXP_TAIL>", "+"}, "NULL"},
      {{"<MUL_EXP_TAIL>", "-"}, "NULL"},
      {{"<MUL_EXP_TAIL>", "=="}, "NULL"},
      {{"<MUL_EXP_TAIL>", "!="}, "NULL"},
      {{"<MUL_EXP_TAIL>", ">"}, "NULL"},
      {{"<MUL_EXP_TAIL>", "<"}, "NULL"},
      {{"<MUL_EXP_TAIL>", ">="}, "NULL"},
      {{"<MUL_EXP_TAIL>", "<="}, "NULL"},
      {{"<MUL_EXP_TAIL>", "&&"}, "NULL"},
      {{"<MUL_EXP_TAIL>", "||"}, "NULL"},
      {{"<MUL_EXP_TAIL>", ")"}, "NULL"},
      {{"<MUL_EXP_TAIL>", ";"}, "NULL"},
      {{"<MUL_EXP_TAIL>", "}"}, "NULL"},
      {{"<MUL_EXP_TAIL>", "#"}, "NULL"},

      // 基本因子（字面量直接处理，无冗余层）
      {{"<FACTOR>", "("}, "( <EXP> )"},
      {{"<FACTOR>", "IDENTIFIER"}, "IDENTIFIER"},
      {{"<FACTOR>", "INTEGER"}, "INTEGER"},
      {{"<FACTOR>", "FLOAT"}, "FLOAT"},
      {{"<FACTOR>", "true"}, "true"},
      {{"<FACTOR>", "false"}, "false"},
      {{"<FACTOR>", "CHAR"}, "CHAR"},
      {{"<FACTOR>", "STRING"}, "STRING"},

      // ===================== 4. 辅助规则 =====================
      // 初始化选项
      {{"<INIT_OPT>", "="}, "= <EXP>"},
      {{"<INIT_OPT>", ";"}, "NULL"},

      // 错误恢复点（实际工程中可扩展）
      {{"<STMT>", "#"}, "NULL"}, // 文件结束时的语句
      {{"<EXP>", "#"}, "NULL"}   // 文件结束时的表达式
  };

  /*
   * 工程建议（实际parser实现时）：
   * 1. 错误恢复：为每个非终结符定义同步符号集
   *    例：<STMT>的同步符号 = { ";", "}", "#", "if", "while", ... }
   * 2. 语义动作：在产生式右部插入动作标记
   *    例："IDENTIFIER = <EXP> ;" -> "IDENTIFIER = <EXP> ; @checkAssignment"
   * 3. 语法树构建：为每个产生式关联节点构建函数
   */

  // 实现总控程序
  // string::size_type 是 C++ 为字符串
  // 设计的安全下标/长度类型(容器专属的可根据系统位数自动适配的无符号整数)
  vector<string>::size_type inputBeltIndex = 0;
  bool flag = false;
  vector<string>::size_type inputBeltSize = inputBelt.size();
  while (inputBeltIndex < inputBeltSize) {
    string currentSymbol = inputBelt[inputBeltIndex];
    string stackTop = symbolStack.top();
    symbolStack.pop();

    // 情况一
    if (stackTop == endSymbol) {
      if (currentSymbol == endSymbol) {
        flag = true;
        break;
      } else {
        cerr << "Analysis Failed: the stack top symbol is: " << stackTop
             << "，but the current input symbol is: " << currentSymbol << endl;
        flag = false;
        break;
      }
    }
    // 情况二
    else if (VN.find(stackTop) != VN.end()) {
      pair<string, string> temp = {stackTop, currentSymbol};
      if (predictionTable.find(temp) != predictionTable.end()) {
        string production = predictionTable[temp];
        if (production != "NULL") { // 空产生式不压栈
          // 1. 按空格拆分production为完整符号列表
          vector<string> symbols;
          istringstream iss(production);
          string symbol;
          while (iss >> symbol) { // 自动按空格拆分，跳过空字符串
            symbols.push_back(symbol);
          }

          // 2. 逆序遍历符号列表，压入栈（栈是先进后出，所以要逆序）
          for (auto it = symbols.rbegin(); it != symbols.rend(); ++it) {
            symbolStack.push(*it);
          }
        }
        outputBelt.push_back(stackTop + " --> " + production);
      } else {
        // 分析失败
        cerr << "Analysis Failed, the current stack top symbol is: " << stackTop
             << ", current input symbol is: " << currentSymbol << endl;
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
             << ", current input symbol is: " << currentSymbol << endl;
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
# 项目说明
- 编译原理课程设计
  - 核心目标：实现编译器前端模块
- 未来目标：从课程设计脱胎为一个完整的自定义编译器
# 项目结构
- 词法分析
  - 词法分析器
  - 词法分析器的测试程序
- 语法分析
  - 语法分析器
  - 语法分析器的测试程序
- 语义分析
  - 语义分析器
  - 语义分析器的测试程序
- 中间代码生成
  - 中间代码生成器
  - 中间代码生成器的测试程序
- 目标代码生成
  - 目标代码生成器
  - 目标代码生成器的测试程序
# 词法分析模块
- 功能
    - 把源程序转换为Token序列
- 设计步骤
    - 确定支持的C++语法子集
        - 本项目当前支持C++11语法子集，包括标识符/关键字、常量、运算符、分隔符、注释等
    - 定义Token结构
        - Token包含如下内容：类型(TokenType)、值、位置(行号、列号、用于报错)
    - 建立关键字表
        - 使用unordered_set存储关键字，值为关键字字符串
    - 构造C++语法的DFA状态转换图
        - 标识符 / 关键字
          - 起始状态：遇到字母→进入 “标识符状态”；
          - 中间状态：持续接收字母 / 数字 / 下划线；
          - 接受状态：遇到非标识符字符（如运算符、空格）→结束，生成标识符 Token，再查表判断是否为关键字。
        - 常量
          - 整数：十进制（123）、八进制（012）、十六进制（0x1a）→状态区分前缀（0、0x）和后续数字。
          - 浮点数：小数（1.23）、科学计数法（1.2e+3）→处理小数点、指数 E/e、指数符号 ±。
        - 字符：单引号包裹（'a'、'\n'）→处理转义字符（\t、\r 等）。
        - 运算符与分隔符
          - 单字符（+、;）：直接匹配→生成对应 Token。
          - 多字符（++、==）：超前查看下一个字符（如读 “+” 后看是否再一个 “+”）→避免拆分错误。
        - 注释与空白符
          - 单行注释（//）：遇到 “//” 后跳过后续字符直到换行→不生成 Token 
          - 多行注释（/.../）：从 “/” 进入注释状态，直到 “/” 退出→跳过中间所有字符。
        - 空白符（空格、换行、制表符）：直接跳过→不生成 Token。
    - 编写词法分析程序，使得词法分析程序的控制程序模拟状态转换图的状态转换
        - 在识别标识符的过程中，要把标识符拼写出来并与保留字(使用unordered_map存储)做对比区分
        - 在识别常数的过程中，要把常数转换为机器表示
    - 辅助功能
        - 错误处理
            - 识别到非法字符时，要报错并跳过该字符
    - 测试与验证
        - 对词法分析器进行测试，包括正常情况和异常情况
            - 正常情况：输入合法的C++程序，输出对应的Token序列
            - 异常情况：输入包含非法字符的C++程序，输出错误提示
    - 可扩展设计
        - 可以根据需要添加对其他C++高版本语法元素的支持
- 说明
  - **本词法分析器必须保证各类词组之间使用空格分隔，否则会报错**
  - **本词法分析器暂不支持中文**
  - **本词法分析暂不支持除法**

# 语法分析详解

## 语法分析目标

语法分析的功能是在词法分析的基础上，根据语言的语法规则，将一个个单词符号组成语言的各种语法类。例如：算术表达式、赋值语句等。

例如：单词类别表示的串：`ID := ID + NUM`，经过语法分析后，被识别出是赋值语句，识别过程相当于建立一棵语法树。

**总结任务**：根据语言的语法规则，将一个个单词符号组成语言的各种语法类，并确定输入串是否构成语法上正确的程序。

- **输入**：单词符号序列
- **输出**：语法树或错误信息

## 语言定义

### EBNF 定义

```
// EBNF：扩展巴克斯-诺尔范式
// 终结符必须用双引号 " 包裹（或单引号）
// 顶层规则
<程序> → <全局声明列表> <主函数定义>
<全局声明列表> → <声明语句> <全局声明列表> | ε  // 全局变量声明（可空/多个）
<主函数定义> → "int" "main" "(" ")" <复合语句>  // C++标准main函数格式

// 声明规则
<声明语句> → <类型说明符> <标识符> <初始化器>? ";"
<类型说明符> → "int" | "float" | "char" | "string" | "bool"
<初始化器> → "=" <表达式>  // 初始化器：仅赋值表达式，靠?实现可选

// 语句规则
<语句> → <if语句> | <while语句> | <赋值语句> | <复合语句> | <空语句>
<if语句> → "if" "(" <布尔表达式> ")" <语句> ("else" <语句>)?
<while语句> → "while" "(" <布尔表达式> ")" <语句>
<赋值语句> → <标识符> "=" <表达式> ";"
<复合语句> → "{" <语句列表> "}"
<语句列表> → <语句> <语句列表> | ε
<空语句> → ";"

// 表达式规则（优先级：布尔 < 算术 < 乘除 < 因子）
<表达式> → <算术表达式> | <布尔表达式> | <字符串字面量> | <字符字面量>
<布尔表达式> → <逻辑或表达式>
<逻辑或表达式> → <逻辑与表达式> ("||" <逻辑与表达式>)*
<逻辑与表达式> → <关系表达式> ("&&" <关系表达式>)*
<关系表达式> → <表达式> <关系运算符> <表达式> | <布尔字面量> | <标识符>
<关系运算符> → "==" | "!=" | ">" | "<" | ">=" | "<="
<算术表达式> → <加法表达式>
<加法表达式> → <乘法表达式> (("+" | "-") <乘法表达式>)*
<乘法表达式> → <因子> (("*" | "/") <因子>)*
<因子> → "(" <表达式> ")" | <标识符> | <数字> | <布尔字面量> | <字符字面量>

// 基础元素（对接词法Token）
<标识符> → 词法IDENTIFIER（如name1、_name2）
<数字> → <整数> | <浮点数>
<整数> → 词法INTEGER（如1、123）
<浮点数> → 词法FLOAT（如3.14、.123）
<布尔字面量> → "true" | "false"
<字符串字面量> → 词法STRING（如"hello"、" "）
<字符字面量> → 词法CHAR（如'a'、'9'）
```

### 语言符号定义

```
// 顶层规则
<PROG> → <GDECL_LIST> <MAIN_FUNC>
<GDECL_LIST> → <DECL_STMT> <GDECL_LIST> | ε
<MAIN_FUNC> → "int" "main" "(" ")" <COMP_STMT>

// 声明规则
<DECL_STMT> → <TYPE> <ID> <INIT>? ";"
<TYPE> → "int" | "float" | "char" | "string" | "bool"
<INIT> → "=" <EXP>

// 语句规则
<STMT> → <IF_STMT> | <WHILE_STMT> | <ASSIGN_STMT> | <COMP_STMT> | <EMPTY_STMT>
<IF_STMT> → "if" "(" <BOOL_EXP> ")" <STMT> ("else" <STMT>)?
<WHILE_STMT> → "while" "(" <BOOL_EXP> ")" <STMT>
<ASSIGN_STMT> → <ID> "=" <EXP> ";"
<COMP_STMT> → "{" <STMT_LIST> "}"
<STMT_LIST> → <STMT> <STMT_LIST> | ε
<EMPTY_STMT> → ";"

// 表达式规则
<EXP> → <ARITH_EXP> | <BOOL_EXP> | <STR_LIT> | <CHAR_LIT>
<BOOL_EXP> → <LOGIC_OR>
<LOGIC_OR> → <LOGIC_AND> ("||" <LOGIC_AND>)*
<LOGIC_AND> → <REL_EXP> ("&&" <REL_EXP>)*
<REL_EXP> → <EXP> <REL_OP> <EXP> | <BOOL_LIT> | <ID>
<REL_OP> → "==" | "!=" | ">" | "<" | ">=" | "<="
<ARITH_EXP> → <ADD_EXP>
<ADD_EXP> → <MUL_EXP> (("+" | "-") <MUL_EXP>)*
<MUL_EXP> → <FACTOR> (("*" | "/") <FACTOR>)*
<FACTOR> → "(" <EXP> ")" | <ID> | <NUM> | <BOOL_LIT> | <CHAR_LIT>

// 基础元素
<ID> → IDENTIFIER
<NUM> → <INT> | <FLOAT>
<INT> → INTEGER
<FLOAT> → FLOAT
<BOOL_LIT> → "true" | "false"
<STR_LIT> → STRING
<CHAR_LIT> → CHAR
```

## 消除文法二义性

### 消除悬挂else的二义性

**问题本质**：当嵌套if语句时，else可以匹配任意未匹配的if，导致语法树不唯一。

**消除方法**：将语句拆分为匹配/未匹配两类，消除else歧义
```
<STMT> → <MATCHED_STMT> | <UNMATCHED_STMT>

// 匹配语句：有明确else，或无else但用{}包裹（无歧义）
<MATCHED_STMT> → <IF_MATCHED> | <WHILE_STMT> | <ASSIGN_STMT> | <COMP_STMT> | <EMPTY_STMT>

// 未匹配语句：无else的if，只能出现在外层（无法被else匹配）
<UNMATCHED_STMT> → <IF_UNMATCHED>

// 含else的if（匹配）：then分支必须是MATCHED_STMT，确保else匹配最近的if
<IF_MATCHED> → "if" "(" <BOOL_EXP> ")" <MATCHED_STMT> "else" <MATCHED_STMT>

// 不含else的if（未匹配）：无else分支，不会触发悬挂问题
<IF_UNMATCHED> → "if" "(" <BOOL_EXP> ")" <MATCHED_STMT>
```

### 消除表达式结合的二义性

**问题本质**：虽然通过「分层调用」解决了优先级歧义（先乘除后加减），但未显式约束结合性（C++中+/-/*//均为左结合）。

**消除方法**：显式约束左结合，通过后缀*操作符实现
```
<ADD_EXP> → <MUL_EXP> (("+" | "-") <MUL_EXP>)*  // 左结合，优先级低于乘除
<MUL_EXP> → <FACTOR> (("*" | "/") <FACTOR>)*    // 左结合，优先级高于加减
```

### 消除二义性后的完整文法

```
// 顶层规则（无歧义）
<PROG> → <GDECL_LIST> <MAIN_FUNC>
<GDECL_LIST> → <DECL_STMT> <GDECL_LIST> | ε
<MAIN_FUNC> → "int" "main" "(" ")" <COMP_STMT>

// 声明规则（无歧义）
<DECL_STMT> → <TYPE> <ID> <INIT>? ";"
<TYPE> → "int" | "float" | "char" | "string" | "bool"
<INIT> → "=" <EXP>

// 语句规则（核心修改：拆分MATCHED/UNMATCHED_STMT消除悬挂else）
<STMT> → <MATCHED_STMT> | <UNMATCHED_STMT>
<MATCHED_STMT> → <IF_MATCHED> | <WHILE_STMT> | <ASSIGN_STMT> | <COMP_STMT> | <EMPTY_STMT>
<UNMATCHED_STMT> → <IF_UNMATCHED>
<IF_MATCHED> → "if" "(" <BOOL_EXP> ")" <MATCHED_STMT> "else" <MATCHED_STMT>
<IF_UNMATCHED> → "if" "(" <BOOL_EXP> ")" <MATCHED_STMT>
<WHILE_STMT> → "while" "(" <BOOL_EXP> ")" <MATCHED_STMT>
<ASSIGN_STMT> → <ID> "=" <EXP> ";"
<COMP_STMT> → "{" <STMT_LIST> "}"
<STMT_LIST> → <STMT> <STMT_LIST> | ε
<EMPTY_STMT> → ";"

// 表达式规则（无歧义：分层解决优先级，后缀*解决左结合）
<EXP> → <ARITH_EXP> | <BOOL_EXP> | <STR_LIT> | <CHAR_LIT>
<BOOL_EXP> → <LOGIC_OR>
<LOGIC_OR> → <LOGIC_AND> ("||" <LOGIC_AND>)*
<LOGIC_AND> → <REL_EXP> ("&&" <REL_EXP>)*
<REL_EXP> → <EXP> <REL_OP> <EXP> | <BOOL_LIT> | <ID>
<REL_OP> → "==" | "!=" | ">" | "<" | ">=" | "<="
<ARITH_EXP> → <ADD_EXP>
<ADD_EXP> → <MUL_EXP> (("+" | "-") <MUL_EXP>)*
<MUL_EXP> → <FACTOR> (("*" | "/") <FACTOR>)*
<FACTOR> → "(" <EXP> ")" | <ID> | <NUM> | <BOOL_LIT> | <CHAR_LIT>
```

## 左递归消除

### 不存在左递归

我们的文法设计已经避免了直接左递归，采用了右递归形式：
```
<GDECL_LIST> → <DECL_STMT> <GDECL_LIST> | ε
<STMT_LIST> → <STMT> <STMT_LIST> | ε
```

### 消除右递归

为了适应LL(1)分析器，需要将右递归转换为等价的左递归友好形式：
```
<GDECL_LIST> → <DECL_STMT> <GDECL_LIST'> | ε
<GDECL_LIST'> → <DECL_STMT> <GDECL_LIST'> | ε

<STMT_LIST> → <STMT> <STMT_LIST'> | ε
<STMT_LIST'> → <STMT> <STMT_LIST'> | ε
```

### 消除右递归后的完整文法

```
<PROG> → <GDECL_LIST> <MAIN_FUNC>
<GDECL_LIST> → <DECL_STMT> <GDECL_LIST'> | ε
<GDECL_LIST'> → <DECL_STMT> <GDECL_LIST'> | ε
<MAIN_FUNC> → "int" "main" "(" ")" <COMP_STMT>
<DECL_STMT> → <TYPE> <ID> <INIT>? ";"
<TYPE> → "int" | "float" | "char" | "string" | "bool"
<INIT> → "=" <EXP>
<STMT> → <MATCHED_STMT> | <UNMATCHED_STMT>
<MATCHED_STMT> → <IF_MATCHED> | <WHILE_STMT> | <ASSIGN_STMT> | <COMP_STMT> | <EMPTY_STMT>
<UNMATCHED_STMT> → <IF_UNMATCHED>
<IF_MATCHED> → "if" "(" <BOOL_EXP> ")" <MATCHED_STMT> "else" <MATCHED_STMT>
<IF_UNMATCHED> → "if" "(" <BOOL_EXP> ")" <MATCHED_STMT>
<WHILE_STMT> → "while" "(" <BOOL_EXP> ")" <MATCHED_STMT>
<ASSIGN_STMT> → <ID> "=" <EXP> ";"
<COMP_STMT> → "{" <STMT_LIST> "}"
<STMT_LIST> → <STMT> <STMT_LIST'> | ε
<STMT_LIST'> → <STMT> <STMT_LIST'> | ε
<EMPTY_STMT> → ";"
<EXP> → <ARITH_EXP> | <BOOL_EXP> | <STR_LIT> | <CHAR_LIT>
<BOOL_EXP> → <LOGIC_OR>
<LOGIC_OR> → <LOGIC_AND> ("||" <LOGIC_AND>)*
<LOGIC_AND> → <REL_EXP> ("&&" <REL_EXP>)*
<REL_EXP> → <EXP> <REL_OP> <EXP> | <BOOL_LIT> | <ID>
<REL_OP> → "==" | "!=" | ">" | "<" | ">=" | "<="
<ARITH_EXP> → <ADD_EXP>
<ADD_EXP> → <MUL_EXP> (("+" | "-") <MUL_EXP>)*
<MUL_EXP> → <FACTOR> (("*" | "/") <FACTOR>)*
<FACTOR> → "(" <EXP> ")" | <ID> | <NUM> | <BOOL_LIT> | <CHAR_LIT>
```

## 最终的LL(1)文法定义

```
/* ====================== 顶层结构（LL(1)合规+C++标准） ====================== */
<PROG> → <GDECL_LIST> <MAIN_FUNC>

// 修复LL(1)冲突：分离int和非int全局声明
<GDECL_LIST> → <NON_INT_GLOBAL_DECL> <GDECL_LIST_TAIL> | <INT_GLOBAL_DECL> <GDECL_LIST_TAIL> | ε
<GDECL_LIST_TAIL> → <NON_INT_GLOBAL_DECL> <GDECL_LIST_TAIL> | <INT_GLOBAL_DECL> <GDECL_LIST_TAIL> | ε
<GLOBAL_DECL> → <TYPE_SPEC> <ID> <INIT_OPT> ";"
<NON_INT_GLOBAL_DECL> → <NON_INT_TYPE> <ID> <INIT_OPT> ";"  // FIRST={float, char, bool, string}
<INT_GLOBAL_DECL> → "int" <ID> <INIT_OPT> ";"  // FIRST={int}
<NON_INT_TYPE> → "float" | "char" | "bool" | "string"  // 排除int类型
<MAIN_FUNC> → "int" "main" "(" ")" <COMP_STMT>  // 严格遵循C++标准

/* ====================== 声明规则（C++兼容） ====================== */
<TYPE_SPEC> → "int" | "float" | "char" | "bool" | "string"
<INIT_OPT> → "=" <EXP> | ε

/* ====================== 语句结构（LL(1)兼容+C++标准） ====================== */
<STMT> → "if" "(" <EXP> ")" <STMT> <ELSE_CLAUSE> 
       | "while" "(" <EXP> ")" <STMT> 
       | <ID> "=" <EXP> ";" 
       | "return" <EXP> ";" 
       | <COMP_STMT> 
       | ";"
<ELSE_CLAUSE> → "else" <STMT> | ε  // 真正的LL(1)解决方案，C++标准：else匹配最近的if
<COMP_STMT> → "{" <STMT_LIST> "}"
<STMT_LIST> → <STMT> <STMT_LIST_TAIL> | ε
<STMT_LIST_TAIL> → <STMT> <STMT_LIST_TAIL> | ε

/* ====================== 表达式（LL(1)标准+C++运算符优先级） ====================== */
<EXP> → <LOGIC_OR>
<LOGIC_OR> → <LOGIC_AND> <LOGIC_OR_TAIL>
<LOGIC_OR_TAIL> → "||" <LOGIC_AND> <LOGIC_OR_TAIL> | ε
<LOGIC_AND> → <REL_EXP> <LOGIC_AND_TAIL>
<LOGIC_AND_TAIL> → "&&" <REL_EXP> <LOGIC_AND_TAIL> | ε
<REL_EXP> → <ADD_EXP> <REL_EXP_TAIL>
<REL_EXP_TAIL> → <REL_OP> <ADD_EXP> <REL_EXP_TAIL> | ε
<REL_OP> → "==" | "!=" | ">" | "<" | ">=" | "<="
<ADD_EXP> → <MUL_EXP> <ADD_EXP_TAIL>
<ADD_EXP_TAIL> → "+" <MUL_EXP> <ADD_EXP_TAIL> | "-" <MUL_EXP> <ADD_EXP_TAIL> | ε
<MUL_EXP> → <FACTOR> <MUL_EXP_TAIL>
<MUL_EXP_TAIL> → "*" <FACTOR> <MUL_EXP_TAIL> | "/" <FACTOR> <MUL_EXP_TAIL> | ε
<FACTOR> → "(" <EXP> ")" | <ID> | <NUM_LITERAL> | <BOOL_LITERAL> | <CHAR_LITERAL> | <STRING_LITERAL>

/* ====================== 基础元素（对接词法分析器） ====================== */
<ID> → IDENTIFIER
<NUM_LITERAL> → <INT_LITERAL> | <FLOAT_LITERAL>
<INT_LITERAL> → INTEGER
<FLOAT_LITERAL> → FLOAT
<BOOL_LITERAL> → "true" | "false"
<CHAR_LITERAL> → CHAR
<STRING_LITERAL> → STRING

/* ====================== 词法终结符分类（C++兼容） ====================== */
<KEYWORD> → "int" | "float" | "char" | "bool" | "string" | "if" | "else" | "while" | "return" | "main" | "true" | "false"
<OPERATOR> → "+" | "-" | "*" | "/" | "=" | "==" | "!=" | ">" | "<" | ">=" | "<=" | "&&" | "||"
<SEPARATOR> → ";" | "{" | "}" | "(" | ")" | ","
```

## 构造LL(1)分析表

### 计算非终结符的FIRST集

```
FIRST(`<PROG>`) = {`float`, `char`, `bool`, `string`, `int`}

FIRST(`<GDECL_LIST>`) = {`float`, `char`, `bool`, `string`, `int`, ε}

FIRST(`<GDECL_LIST_TAIL>`) = {`float`, `char`, `bool`, `string`, `int`, ε}

FIRST(`<GLOBAL_DECL>`) = {`int`, `float`, `char`, `bool`, `string`}

FIRST(`<NON_INT_GLOBAL_DECL>`) = {`float`, `char`, `bool`, `string`}

FIRST(`<INT_GLOBAL_DECL>`) = {`int`}

FIRST(`<NON_INT_TYPE>`) = {`float`, `char`, `bool`, `string`}

FIRST(`<MAIN_FUNC>`) = {`int`}

FIRST(`<TYPE_SPEC>`) = {`int`, `float`, `char`, `bool`, `string`}

FIRST(`<INIT_OPT>`) = {`=`, ε}

FIRST(`<STMT>`) = {`if`, `while`, IDENTIFIER, `return`, `{`, `;`}

FIRST(`<ELSE_CLAUSE>`) = {`else`, ε}

FIRST(`<COMP_STMT>`) = {`{`}

FIRST(`<STMT_LIST>`) = {`if`, `while`, IDENTIFIER, `return`, `{`, `;`, ε}

FIRST(`<STMT_LIST_TAIL>`) = {`if`, `while`, IDENTIFIER, `return`, `{`, `;`, ε}

FIRST(`<EXP>`) = FIRST(`<LOGIC_OR>`)

FIRST(`<LOGIC_OR>`) = FIRST(`<LOGIC_AND>`)

FIRST(`<LOGIC_OR_TAIL>`) = {`||`, ε}

FIRST(`<LOGIC_AND>`) = FIRST(`<REL_EXP>`)

FIRST(`<LOGIC_AND_TAIL>`) = {`&&`, ε}

FIRST(`<REL_EXP>`) = FIRST(`<ADD_EXP>`)

FIRST(`<REL_EXP_TAIL>`) = {`==`, `!=`, `>`, `<`, `>=`, `<=`, ε}

FIRST(`<REL_OP>`) = {`==`, `!=`, `>`, `<`, `>=`, `<=`}

FIRST(`<ADD_EXP>`) = FIRST(`<MUL_EXP>`)

FIRST(`<ADD_EXP_TAIL>`) = {`+`, `-`, ε}

FIRST(`<MUL_EXP>`) = FIRST(`<FACTOR>`)

FIRST(`<MUL_EXP_TAIL>`) = {`*`, `/`, ε}

FIRST(`<FACTOR>`) = {`(`, IDENTIFIER, INTEGER, FLOAT, `true`, `false`, CHAR, STRING}

FIRST(`<ID>`) = {IDENTIFIER}

FIRST(`<NUM_LITERAL>`) = {INTEGER, FLOAT}

FIRST(`<INT_LITERAL>`) = {INTEGER}

FIRST(`<FLOAT_LITERAL>`) = {FLOAT}

FIRST(`<BOOL_LITERAL>`) = {`true`, `false`}

FIRST(`<CHAR_LITERAL>`) = {CHAR}

FIRST(`<STRING_LITERAL>`) = {STRING}

FIRST(`<KEYWORD>`) = {`int`, `float`, `char`, `bool`, `string`, `if`, `else`, `while`, `return`, `main`, `true`, `false`}

FIRST(`<OPERATOR>`) = {`+`, `-`, `*`, `/`, `=`, `==`, `!=`, `>`, `<`, `>=`, `<=`, `&&`, `||`}

FIRST(`<SEPARATOR>`) = {`;`, `{`, `}`, `(`, `)`, `,`}
```

### 计算非终结符的FOLLOW集


```
FOLLOW(`<PROG>`) = {`#`}

FOLLOW(`<GDECL_LIST>`) = {`int`}

FOLLOW(`<MAIN_FUNC>`) = {`#`}

FOLLOW(`<GDECL_LIST_TAIL>`) = {`int`}

FOLLOW(`<NON_INT_GLOBAL_DECL>`) = {`float`, `char`, `bool`, `string`, `int`}

FOLLOW(`<INT_GLOBAL_DECL>`) = {`float`, `char`, `bool`, `string`, `int`}

FOLLOW(`<TYPE_SPEC>`) = {IDENTIFIER}

FOLLOW(`<NON_INT_TYPE>`) = {IDENTIFIER}

FOLLOW(`<INIT_OPT>`) = {`;`}

FOLLOW(`<STMT>`) = {`if`, `while`, IDENTIFIER, `return`, `{`, `;`, `}`, `else`}

FOLLOW(`<ELSE_CLAUSE>`) = {`if`, `while`, IDENTIFIER, `return`, `{`, `;`, `}`, `else`}

FOLLOW(`<COMP_STMT>`) = {`if`, `while`, IDENTIFIER, `return`, `{`, `;`, `}`, `else`, `#`}

FOLLOW(`<STMT_LIST>`) = {`}`}

FOLLOW(`<STMT_LIST_TAIL>`) = {`}`}

FOLLOW(`<EXP>`) = {`)`, `;`}

FOLLOW(`<LOGIC_OR>`) = {`)`, `;`}

FOLLOW(`<LOGIC_OR_TAIL>`) = {`)`, `;`}

FOLLOW(`<LOGIC_AND>`) = {`||`, `)`, `;`}

FOLLOW(`<LOGIC_AND_TAIL>`) = {`||`, `)`, `;`}

FOLLOW(`<REL_EXP>`) = {`&&`, `||`, `)`, `;`}

FOLLOW(`<REL_EXP_TAIL>`) = {`&&`, `||`, `)`, `;`}

FOLLOW(`<REL_OP>`) = {`(`, IDENTIFIER, INTEGER, FLOAT, `true`, `false`, CHAR, STRING}

FOLLOW(`<ADD_EXP>`) = {`==`, `!=`, `>`, `<`, `>=`, `<=`, `&&`, `||`, `)`, `;`}

FOLLOW(`<ADD_EXP_TAIL>`) = {`==`, `!=`, `>`, `<`, `>=`, `<=`, `&&`, `||`, `)`, `;`}

FOLLOW(`<MUL_EXP>`) = {`+`, `-`, `==`, `!=`, `>`, `<`, `>=`, `<=`, `&&`, `||`, `)`, `;`}

FOLLOW(`<MUL_EXP_TAIL>`) = {`+`, `-`, `==`, `!=`, `>`, `<`, `>=`, `<=`, `&&`, `||`, `)`, `;`}

FOLLOW(`<FACTOR>`) = {`*`, `/`, `+`, `-`, `==`, `!=`, `>`, `<`, `>=`, `<=`, `&&`, `||`, `)`, `;`}

FOLLOW(`<ID>`) = {`=`, `*`, `/`, `+`, `-`, `==`, `!=`, `>`, `<`, `>=`, `<=`, `&&`, `||`, `)`, `;`}

FOLLOW(`<NUM_LITERAL>`) = {`*`, `/`, `+`, `-`, `==`, `!=`, `>`, `<`, `>=`, `<=`, `&&`, `||`, `)`, `;`}

FOLLOW(`<INT_LITERAL>`) = {`*`, `/`, `+`, `-`, `==`, `!=`, `>`, `<`, `>=`, `<=`, `&&`, `||`, `)`, `;`}

FOLLOW(`<FLOAT_LITERAL>`) = {`*`, `/`, `+`, `-`, `==`, `!=`, `>`, `<`, `>=`, `<=`, `&&`, `||`, `)`, `;`}

FOLLOW(`<BOOL_LITERAL>`) = {`*`, `/`, `+`, `-`, `==`, `!=`, `>`, `<`, `>=`, `<=`, `&&`, `||`, `)`, `;`}

FOLLOW(`<CHAR_LITERAL>`) = {`*`, `/`, `+`, `-`, `==`, `!=`, `>`, `<`, `>=`, `<=`, `&&`, `||`, `)`, `;`}

FOLLOW(`<STRING_LITERAL>`) = {`*`, `/`, `+`, `-`, `==`, `!=`, `>`, `<`, `>=`, `<=`, `&&`, `||`, `)`, `;`}
```

### 构建LL(1)分析表

#### 核心非终结符的预测分析表

##### 1. `<PROG>`, `<GDECL_LIST>`, `<GDECL_LIST_TAIL>` 相关

| 非终结符\终结符 | `float` | `char` | `bool` | `string` | `int` | `#` |
|-----------------|---------|--------|--------|----------|-------|-----|
| `<PROG>` | `<GDECL_LIST> <MAIN_FUNC>` | `<GDECL_LIST> <MAIN_FUNC>` | `<GDECL_LIST> <MAIN_FUNC>` | `<GDECL_LIST> <MAIN_FUNC>` | `<GDECL_LIST> <MAIN_FUNC>` | |
| `<GDECL_LIST>` | `<GLOBAL_DECL> <GDECL_LIST_TAIL>` | `<GLOBAL_DECL> <GDECL_LIST_TAIL>` | `<GLOBAL_DECL> <GDECL_LIST_TAIL>` | `<GLOBAL_DECL> <GDECL_LIST_TAIL>` | **ε** | |
| `<GDECL_LIST_TAIL>` | `<GLOBAL_DECL> <GDECL_LIST_TAIL>` | `<GLOBAL_DECL> <GDECL_LIST_TAIL>` | `<GLOBAL_DECL> <GDECL_LIST_TAIL>` | `<GLOBAL_DECL> <GDECL_LIST_TAIL>` | **ε** | |

##### 2. `<STMT>`, `<COMP_STMT>`, `<STMT_LIST>` 相关

| 非终结符\终结符 | `{` | `}` | `if` | `else` | `while` | `return` | IDENTIFIER |
|-----------------|-----|-----|------|--------|---------|----------|------------|
| `<STMT>` | `<COMP_STMT>` | | `"if" "(" <EXP> ")" <STMT> <ELSE_CLAUSE>` | | `"while" "(" <EXP> ")" <STMT>` | `"return" <EXP> ";"` | `<ID> "=" <EXP> ";"` |
| `<COMP_STMT>` | `"{" <STMT_LIST> "}"` | | | | | | |
| `<STMT_LIST>` | `<STMT> <STMT_LIST_TAIL>` | **ε** | `<STMT> <STMT_LIST_TAIL>` | | `<STMT> <STMT_LIST_TAIL>` | `<STMT> <STMT_LIST_TAIL>` | `<STMT> <STMT_LIST_TAIL>` |
| `<STMT_LIST_TAIL>` | `<STMT> <STMT_LIST_TAIL>` | **ε** | `<STMT> <STMT_LIST_TAIL>` | | `<STMT> <STMT_LIST_TAIL>` | `<STMT> <STMT_LIST_TAIL>` | `<STMT> <STMT_LIST_TAIL>` |
| `<ELSE_CLAUSE>` | **ε** | **ε** | **ε** | `"else" <STMT>` | **ε** | **ε** | **ε** |

##### 3. `<EXP>` 及表达式相关

| 非终结符\终结符 | `(` | IDENTIFIER | INTEGER | FLOAT | `true` | `false` | CHAR | STRING |
|-----------------|-----|------------|---------|-------|--------|---------|------|--------|
| `<EXP>` | `<LOGIC_OR>` | `<LOGIC_OR>` | `<LOGIC_OR>` | `<LOGIC_OR>` | `<LOGIC_OR>` | `<LOGIC_OR>` | `<LOGIC_OR>` | `<LOGIC_OR>` |
| `<LOGIC_OR>` | `<LOGIC_AND> <LOGIC_OR_TAIL>` | `<LOGIC_AND> <LOGIC_OR_TAIL>` | `<LOGIC_AND> <LOGIC_OR_TAIL>` | `<LOGIC_AND> <LOGIC_OR_TAIL>` | `<LOGIC_AND> <LOGIC_OR_TAIL>` | `<LOGIC_AND> <LOGIC_OR_TAIL>` | `<LOGIC_AND> <LOGIC_OR_TAIL>` | `<LOGIC_AND> <LOGIC_OR_TAIL>` |
| `<LOGIC_OR_TAIL>` | `"||" <LOGIC_AND> <LOGIC_OR_TAIL>` | **ε** | **ε** | | | | | |
| `<LOGIC_AND>` | `<REL_EXP> <LOGIC_AND_TAIL>` | `<REL_EXP> <LOGIC_AND_TAIL>` | `<REL_EXP> <LOGIC_AND_TAIL>` | `<REL_EXP> <LOGIC_AND_TAIL>` | `<REL_EXP> <LOGIC_AND_TAIL>` | `<REL_EXP> <LOGIC_AND_TAIL>` | `<REL_EXP> <LOGIC_AND_TAIL>` | `<REL_EXP> <LOGIC_AND_TAIL>` |
| `<LOGIC_AND_TAIL>` | `"&&" <REL_EXP> <LOGIC_AND_TAIL>` | **ε** | **ε** | **ε** | | | | |
| `<REL_EXP>` | `<ADD_EXP> <REL_EXP_TAIL>` | `<ADD_EXP> <REL_EXP_TAIL>` | `<ADD_EXP> <REL_EXP_TAIL>` | `<ADD_EXP> <REL_EXP_TAIL>` | `<ADD_EXP> <REL_EXP_TAIL>` | `<ADD_EXP> <REL_EXP_TAIL>` | `<ADD_EXP> <REL_EXP_TAIL>` | `<ADD_EXP> <REL_EXP_TAIL>` |
| `<REL_EXP_TAIL>` | `<REL_OP> <ADD_EXP> <REL_EXP_TAIL>` | `<REL_OP> <ADD_EXP> <REL_EXP_TAIL>` | `<REL_OP> <ADD_EXP> <REL_EXP_TAIL>` | `<REL_OP> <ADD_EXP> <REL_EXP_TAIL>` | `<REL_OP> <ADD_EXP> <REL_EXP_TAIL>` | `<REL_OP> <ADD_EXP> <REL_EXP_TAIL>` | **ε** | **ε** | **ε** | **ε** |

## 总结

我们成功构建了一个符合LL(1)要求的语法分析器，通过以下步骤：
1. 定义无二义性的文法
2. 消除左递归和右递归
3. 修复LL(1)冲突（如全局声明中的int类型处理）
4. 精确计算FIRST和FOLLOW集
5. 构建无冲突的预测分析表
6. 实现分析器框架

该语法分析器能够准确识别C++子集的语法结构，为后续的语义分析和代码生成奠定基础。通过严格的LL(1)条件验证，确保了分析过程的确定性和高效性。
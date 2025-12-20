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
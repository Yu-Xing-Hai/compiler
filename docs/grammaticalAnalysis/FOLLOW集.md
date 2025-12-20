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
#include "lex.h"
#include <unordered_map>

unordered_map<int, string> convertion;

void init() {
    convertion[LT] = '<';
    convertion[LE] = "<=";
    convertion[EQ] = "==";
    convertion[GT] = ">";
    convertion[GE] = ">=";
    convertion[NEQ] = "!=";
    convertion[ASSIGN] = "=";
    convertion[ANNOTATION1] = "//";
    convertion[ADD] = '+';
    convertion[SUB] = '-';
    convertion[MUL] = '*';
    convertion[DIV] = '/';
    convertion[LEFT_BRACKET] = '(';
    convertion[RIGHT_BRACKET] = ')';
    convertion[EOF] = '#';
}
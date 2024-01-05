#include <cstring>
#include <iostream>
using namespace std;
#define RESERVED_WORDS_LEN 12//保留字数组长度
#define ID_MAX_LEN 32//标识符最大长度
#define ID 0//标识符
#define UNSIGNED_INT 1//无符号整数
#define RESERVED 50//保留字
#define LT 3// '<'
#define LE 4// '<='
#define EQ 5// '=='
#define GT 6// '>'
#define GE 7// '>='
#define NEQ 8// '!='
#define ASSIGN 9// '='
#define ANNOTATION1 10// '// '

#define ANNOTATION2 12// '/* */ '
#define ADD 13// '+'
#define SUB 14// '-'
#define MUL 15// '*'
#define DIV 16// '/'
#define SEMICOLON 17// ';'
#define COMMA 18// ','
#define LEFT_BRACKET 19// '('
#define RIGHT_BRACKET 20// ')'
#define LEFT_BRACE 21// '{'
#define RIGHT_BRACE 22// '}'
#define NOT 23// '!'
#define SELF_ADD 24// '++'
#define SELF_SUB 25// '--'
#define RS 26// '>>'
#define LS 27// '<<'
#define ADDEQ 28// '+='
#define SUBEQ 29// '-='
#define MULEQ 30// '*='
#define DIVEQ 31// '/='
#define AND 32// '&&'
#define OR 33// '||'
#define BIT_AND 34// '&'
#define BIT_OR 35// '|'
#define DOUBLE_QUO 36 // '" '
#define SINGLE_QUO 37 // '''
#define PERCENT 38 // "%"
#define EOF 39 // "#"

#define VOID 51
#define INT 52
#define CHAR 53
#define FLOAT 54
#define DOUBLE 55
#define BOOL 56
#define IF 57
#define ELSE 58
#define FOR 59
#define WHILE 60
#define DO 61
#define RETURN 62
void init();

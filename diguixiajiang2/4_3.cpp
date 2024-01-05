下面的程序是一个递归下降分析程序，它用于分析下面的文法：
〈程序〉-> begin（语句〉end
〈语句）-> <赋值语句＞〈条件语句〉
〈赋值语句〉-> <变量〉： 〈表达式〉
〈条件语句〉->if《表达式>then〈语句）
〈表达式〉-> 变量〉
〈表达式〉 ->〈表达式〉十〈变量〉
〈变量〉->i
以下程序中 Save 表示 save token_pointer value, Restore 表示 restore
token_pointer value.
请你将下述程序改写为一个C语言程序，要求程序能够正确地分析上述文法。
P-begin S endS→A CA→V:=EC→ if E then S
        E-VE' E' →+VE' |eV→I
Function P: boolean;
Begin
        Save;
P:=true:
If next_token=" begin" then
        If S then
If next_token=" end" then return;;
Restore;
P:=false;
End;
        Function A:boolean;
Beign
        Save;
A: =true;
If V then
        If next_token=" :=" then
        If E then return;
Restore;
A: =flase:
End;

Function S: boolean;
Beign
        Save;
        S:=true;
If A then return;
Restore;
If C then return;
Restore;
S:=false;
End;
Function C:boolean;
Begin
        Save;
C:=true:
If next_token=" if" then
        If E then
If next_token=" then"
then
        If S then return;
Restore;
C:=false;
End;
Function E:boolean;
Begin
        Save;
E:=true;
If V then
        If Ep then return;
Restore;
E:=false;
End;
Function Ep:boolean;
Being
        Save;
Ep:=true;
If next_token=' +' then
        If V then
        If Ep then return;
Return;
End;



#include <stdio.h>
#include <stdbool.h>
#include <string.h>

char *tokens[] = {"begin", "end", "if", ":=", "+", "then", "i"};

int token_pointer = 0;

bool matchToken(char *expected) {
    if (strcmp(tokens[token_pointer], expected) == 0) {
        token_pointer++;
        return true;
    }
    return false;
}

bool E();
bool Ep();
bool C();
bool A();
bool S();
bool P();

int main() {
    char input[] = "begin i := i + i end";

    // Tokenizing the input
    char *token = strtok(input, " ");
    int num_tokens = 0;
    char *user_tokens[100];
    while (token != NULL && num_tokens < 100) {
        user_tokens[num_tokens++] = token;
        token = strtok(NULL, " ");
    }

    token_pointer = 0;
    bool valid = P();

    if (valid) {
        printf("Input matches grammar!\n");
    } else {
        printf("Input does not match grammar.\n");
    }

    return 0;
}

bool P() {
    int initial_pointer = token_pointer;
    bool result = true;

    if (matchToken("begin") && S() && matchToken("end")) {
        return true;
    }

    token_pointer = initial_pointer;
    return false;
}

bool S() {
    int initial_pointer = token_pointer;
    bool result = true;

    if (A()) {
        return true;
    }

    token_pointer = initial_pointer;

    if (C()) {
        return true;
    }

    token_pointer = initial_pointer;
    return false;
}

bool A() {
    int initial_pointer = token_pointer;
    bool result = true;

    if (matchToken("i") && matchToken(":=") && E()) {
        return true;
    }

    token_pointer = initial_pointer;
    return false;
}

bool C() {
    int initial_pointer = token_pointer;
    bool result = true;

    if (matchToken("if") && E() && matchToken("then") && S()) {
        return true;
    }

    token_pointer = initial_pointer;
    return false;
}

bool E() {
    int initial_pointer = token_pointer;
    bool result = true;

    if (matchToken("i")) {
        if (Ep()) {
            return true;
        }
    }

    token_pointer = initial_pointer;
    return false;
}

bool Ep() {
    int initial_pointer = token_pointer;
    bool result = true;

    if (matchToken("+") && matchToken("i")) {
        if (Ep()) {
            return true;
        }
    }

    token_pointer = initial_pointer;
    return true; // ε产生式的情况
}

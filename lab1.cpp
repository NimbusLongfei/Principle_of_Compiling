#include <iostream>
#include<string>
#include<fstream>
#include <map>
using namespace std;

#define NOT 1
#define AND 2
#define GA 3
#define PSL 4 //行注释
#define NS 5
#define ID 6
#define INTVALUE 7
#define LT 8
#define LE 9
#define EQ 10
#define NE 11
#define GT 12
#define GE 13
#define LS 14   // 右移
#define RS 15   // 左移
#define LG 16
#define dengyu 17
#define PS 18
#define ADD 19
#define INC 20
#define DEC 21
#define SUB 22
#define ADDE 23
#define SUBE 24
#define DIV 25
#define DIVE 26
#define LP 27   // 左小括号
#define RP 28   // 右小括号
#define OR 29
#define LC 30   // 左花括号
#define RC 31   // 右花括号
#define SE 32   // 分号
#define CO 33   // 逗号
#define NOR 34
#define QUO 35  // 引号
#define PER 36  // 百分号

string keyword[] = {"void", "int", "float", "double", "if", "else",
                    "for", "do", "while",  "return", "include",
                    "printf", "long"};
char TOKEN[20];

int iskeyword(string s){
    for(int i = 0; i < sizeof(keyword); i++){
        if(keyword[i] == s)
            return 1;
    }
    return 0;
}

long long DecToBin(int n){
    long long bin = 0;
    int remainder, i = 1;
    while (n!=0)
    {
        remainder = n%2;
        n /= 2;
        bin += remainder*i;
        i *= 10;
    }
    return bin;
}



int main(void){
    FILE * fp;
    fp = fopen ("/Users/lilongfei/CLionProjects/compile/test5.txt", "r");
    if (fp == NULL) {
        cout << "无法打开文件！" << endl;
        return 1;
    }
    map<string, int> keywordMap;
    keywordMap["void"] = 39;
    keywordMap["int"] = 40;
    keywordMap["float"] = 41;
    keywordMap["double"] = 42;
    keywordMap["if"] = 43;
    keywordMap["else"] = 44;
    keywordMap["for"] = 45;
    keywordMap["do"] = 46;
    keywordMap["while"] = 47;
    keywordMap["long"] = 48;
    keywordMap["return"] = 49;
    keywordMap["include"] = 50;
    keywordMap["printf"] = 51;

    char ch;
    int i;

    do {
        ch = fgetc(fp);
        if(isalpha(ch)) {
            TOKEN[0] = ch;
            ch = fgetc(fp);
            i = 1;
            int c = 1;
            while (isalnum(ch)) {
                TOKEN[i++] = ch;
                ch = fgetc(fp);
            }
            TOKEN[i] = '\0';
            fseek(fp, -1, 1);
            c = iskeyword(TOKEN);
            if (c == 0) cout << "("<< ID <<  ", " << TOKEN << ')' << endl;
            else cout <<"("<< keywordMap[TOKEN] << "," << TOKEN << ")" << endl;

        }else if(isdigit(ch)) {
            TOKEN[0] = ch;
            ch = fgetc(fp);
            i = 1;
            while (isdigit(ch)) {
                TOKEN[i++] = ch;
                ch = fgetc(fp);
            }
            TOKEN[i] = '\0';
            fseek(fp, -1, 1);
            cout << "(" << INTVALUE << "," << DecToBin(stoi(TOKEN)) << ")"<< endl;

        }else{
            switch(ch){
                case '<': ch = fgetc(fp);
                    if(ch == '=') cout << "("<< LE << ", )"<<endl;
                    else if(ch == '<') cout <<"(" << LS << ", )"<<endl;
                    else if (ch == '>') cout <<"(" << LG << ", )"<<endl;
                    else{
                        fseek(fp, -1, 1);
                        cout <<"(" << LT << ", )" << endl;
                    }
                    break;
                case '=': ch = fgetc(fp);
                    if (ch == '=') cout <<"(" << dengyu << ", )"<<endl;
                    else{
                        cout <<"(" << EQ << ", )" << endl;
                        fseek(fp, -1 ,1);
                    }
                    break;
                case '>': ch = fgetc(fp);
                    if(ch == '=') cout << "("<< GE << ", )"<<endl;
                    else if (ch == '>') cout <<"(" << RS << ", )"<<endl;
                    else{
                        fseek(fp, -1, 1);
                        cout <<"(" << GT << ", )" << endl;
                    }
                    break;
                case '!' : ch = fgetc(fp);
                    if(ch == '=')   cout <<"(" << NE << ", )" << endl;
                    else{
                        fseek(fp, -1, 1);
                        cout << "(" << NOT << ", )" << endl;
                    }

                    break;
                case '/': ch = fgetc(fp);
                    if (ch == '*'){
                        cout <<"(" << PS << ", )" << endl;
                        int flag = 0;
                        do {
                            ch = fgetc(fp);
                            if (ch == '*') {
                                ch = fgetc(fp);
                                if (ch == '/') flag = 1;
                            }
                        } while (flag == 0);
                    }
                    else if (ch == '=') cout << '(' << DIVE << ", )" <<endl;
                    else if (ch == '/'){
                        cout <<"(" << PSL << ", )" << endl;
                        do {
                            ch = fgetc(fp);
                            if (ch == '\n') break;
                        } while (1);
                    }
                    else{
                        fseek(fp, -1, 1);
                        cout << '(' << DIV << ", )" <<endl;
                    }
                    break;
                case '+': ch = fgetc(fp);
                    if (ch == '=') cout <<'(' << ADDE << ", )"<< endl;
                    else if(ch == '+') cout<<'(' << INC << ", )" <<endl;
                    else{
                        fseek(fp, -1, 1);
                        cout << '(' << ADD << ", )"<<endl;
                    }
                    break;
                case '-': ch = fgetc(fp);
                    if (ch == '=') cout <<'(' << SUBE << ", )"<< endl;
                    else if(ch == '-') cout<<'(' << DEC << ", )" <<endl;
                    else{
                        fseek(fp, -1, 1);
                        cout << '(' << SUB << ", )"<<endl;
                    }
                    break;
                case '(':
                    cout << '(' << LP << ", )"<<endl;
                    break;
                case ')':
                    cout << '(' << RP << ", )"<<endl;
                    break;
                case '|':
                    ch = fgetc(fp);
                    if(ch == '|')   cout << '(' << OR << ", )"<<endl;
                    else{
                        fseek(fp, -1, 1);
                        cout << '(' << NOR << ", )"<<endl;
                    }
                    break;
                case ';':cout << '(' << SE << ", )"<<endl;
                    break;
                case ',':cout << '(' << CO << ", )"<<endl;
                    break;
                case '&':
                    ch = fgetc(fp);
                    if (ch == '&')cout << '(' << AND << ", )"<<endl;
                    else{
                        fseek(fp, -1, 1);
                        cout << '(' << GA << ", )"<<endl;
                    }
                    break;
                case '{':
                    cout << '(' << LC << ", )"<<endl;
                    break;
                case '}':
                    cout << '(' << RC << ", )"<<endl;
                    break;
                case '"':
                    cout << '(' << QUO << ", )"<<endl;
                    break;
                case '%':
                    cout << '(' << PER << ", )"<<endl;
                    break;
                case '#':
                    cout << '(' << NS << ", )"<<endl;
                    break;
                default:
                    if(ch == ' ') continue;
                    if (ch == '\n') continue;
                    break;
            }
        }
    }while (ch != EOF);
    fclose(fp);
    return 0;
}
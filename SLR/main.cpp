#include <iostream>
#include "SLR.h"
#include "lex.h"
using namespace std;

int main()
{
    init();
    setGrammar();//设置文法
    genFirst();//生成first集
    genFollow();//生成follow集
    showFirst();//显示first集
    showFollow();//显示follow集
    showGenerations();
    genDFA();//生成DFA
    showStates();//显示项目集
    genSLRTable();//生成SLR(1)分析表
    showSLRTable();//显示SLR(1)分析表
    FILE* fp = fopen("./SLR_input.txt", "r");//将词法分析结果作为语法分析的输入
    if (SLR(fp)) {//调用SLR分析函数
        cout << "此语句符合语法规则" << endl;
        cout << "四元式如下所示：" << endl;
        showSiYuanShi();//显示四元式
    }
    else
        cout << "此语句不符合语法规则！" << endl;

    fclose(fp);
    return 0;

}


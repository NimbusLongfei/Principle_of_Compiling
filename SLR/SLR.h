#pragma once
#include <string>
#include <iostream>
#include <set>
#include <map>
#include <unordered_map>
#include <stack>
#define BEGIN 'Q'//文法开始符号
#define MAX_NUM 100
#define EPSILON '$'//空符号
using namespace std;
#pragma warning(disable:4996)

//规定文法的开始符号是S
//G[S]:
//拓广：Q->S
//S->V=E
//E->E+T
//E->E-T
//E->T
//T->T*F
//T->T/F
//T->F
//F->(E)
//F->i
//V->i

//产生式
typedef struct node {
    char left;//产生式左部
    string right;//产生式右部

    bool operator == (const struct node item) const {
        return left == item.left && right == item.right;
    }
}NODE;

//通过状态与符号连接查询SLR分析表元素
typedef struct query {
    int state;//状态
    char ch;//符号
}Query;

//SLR分析表的元素
typedef struct slr {
    char kind;//'r': 归约； 's': 移进； 'a': 接受; 'e': 出错
    int num;//表示用第几个产生式归约或移进的状态数,-1表示接受项目，-2表示出错
}slr;

//插入一条C++ STL的知识点
//map、multimap 容器都会自行根据键的大小对存储的键值对进行排序，set 容器也会如此，
//只不过 set 容器中各键值对的键 key 和值 value 是相等的，
//根据 key 排序，也就等价为根据 value 排序



//项目集的元素类型
typedef struct stateElem {
    bool finish;//表示是否为归约，false表示不是，true表示是
    int pos;//表示圆点的位置，取值从0-gen.size(),当为gen.size()时，表示结束；当前圆点面临的符号为：gen.right[pos]
    NODE gen;//产生式
    int No;//表示第几个产生式,从1开始

    bool operator==(const stateElem& item)const {//等于号重载
        return gen == item.gen && pos == item.pos;
    }
    bool operator<(const stateElem& item)const {//为了对set的key进行排序
        return No < item.No || (No == item.No && pos < item.pos);
    }
    void operator=(const stateElem& item) {//赋值运算符重载
        finish = item.finish;
        pos = item.pos;
        gen = item.gen;
        No = item.No;
    }
}stateElem;

//项目集
typedef struct state {
    int number;//项目编号
    //以下四个成员在closure函数中完成
    set<stateElem> elem;//项目
    set<char> chs;//构造完毕后，项目集面临的所有符号
    bool conflict1;//第一类冲突，即归约与移进并存。表示是否有冲突，false:无冲突；true:有冲突。
    bool conflict2;//第二类冲突，多个归约并存
    bool canShift;//表示是否有后继项目，true表示有，false表示没有。所有项目的finish交在一起若为false，canShift=true,否则为false.
    bool hasReduce;//表示是否有归约式，true表示有，false表示没有
    bool operator==(const state& item)const {
        return elem == item.elem;
    }
}state;

//四元式
typedef struct siYuanShi {
    char op;
    char arg1;
    char arg2;
    char res;
}siYuanShi;

//符号表
typedef map<char, deque<char>> Symbol;

//仿函数
class compareQuery {
public:
    bool operator()(const Query& a, const Query& b) const {
        return a.state < b.state || (a.state == b.state && a.ch < b.ch);//升序
    }
};

//SLR(1)分析过程：生成初态集I0 -> ...DFA -> SLR_table -> SLR -> 四元式的生成？

//SLR分析表, 通过Query找分析表元素
typedef map<Query, slr, compareQuery> SLR_table;

//项目集规范族DFA，通过Query找项目集编号
typedef map<Query, int, compareQuery> DFA;

//通过非终结符号找到其所有产生式右部
typedef unordered_map<char, set<string>> NODES;

//通过非终结符号找到first集合和follow集合
typedef map<char, set<char>> Set;

typedef struct tuple {
    int id;
    string word;
}WORD;//词法分析的二元组结构体

extern unordered_map<int, string> convertion;

void setGrammar();//设置文法NODES
void genFirst();//生成first集
void genFollow();//生成follow集
void showFirst();//显示first集
void showFollow();//显示follow集
void showSLRTable();//显示SLR分析表
void showStates();//显示项目集
void showGenerations();//显示产生式

void genDFA();//生成DFA
bool genSLRTable();//生成SLR分析表,返回true表示此文法为SLR(1)文法
bool SLR(FILE* fp);//SLR分析
void showSiYuanShi();//显示四元式
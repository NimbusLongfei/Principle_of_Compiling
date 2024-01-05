#include "SLR.h"
#include "lex.h"
#include <string>
#include <iostream>

Set first, follow;//first集、follow集
NODES grammar;//文法集合
NODE generations[MAX_NUM];//顺序存放产生式，便于后续填表，下标为产生式编号
set<char> nonTerminate, Terminate;//非终结符号集, 终结符号集
SLR_table table;//分析表
vector<int> state_stack;//状态栈
vector<char> ch_stack;//符号栈
state states[MAX_NUM];//项目集合,下标为项目集编号
Symbol symbol_table;//符号表
vector<siYuanShi> siYuanShis;//四元式集合
int result_count = 0;//四元式个数
int stateNum = 0;//项目集个数
int generationNum = 0;//产生式个数
DFA stateDFA;//识别活前缀的DFA
WORD current = { -1,"" };

#include <cstring>

WORD getCurrent(FILE* fp) {
    current.word.clear();

    char line[256]; // 假设一行不超过 256 个字符
    if (fgets(line, sizeof(line), fp) != NULL) {
        cout << line << endl;
        int id;
        char word[256]; // 假设单词不超过 256 个字符

        sscanf(line, "%d %s", &id, word);

        current.id = id;
        current.word = std::string(word); // 直接赋值给 current.word
    }else {
        current.id = -1;
    }

    return current;
}





//分解产生式，以->为界
NODE splitGeneration(string generation) {
    string right;
    int len = generation.size();
    int pos = 0;
    for (; pos < len; pos++)
        if (generation[pos] == '>') {
            pos++;
            break;//此时找到->后的首个非空字符
        }
    right = generation.substr(pos, len - pos);
    return { generation[0],right };
}

void setGrammar() {//设置文法NODES, 同时收集非终结符号与终结符号
    cout << "请输入文法(中间不要带空格)：" << endl;
    string generation;
    NODE gram;
    while (cin >> generation) {
        if(generation == "end")
            break;
        gram = splitGeneration(generation);//首先分解产生式
        grammar[gram.left].insert(gram.right);//再将此产生式加入产生规则
        nonTerminate.insert(gram.left);//收集非终结符号
        int rightLen = gram.right.size();//产生式右部长度
        for (int i = 0; i < rightLen; i++)
            if (!((gram.right)[i] >= 'A' && (gram.right)[i] <= 'Z'))//不是大写字母就是终结符号
                Terminate.insert(gram.right[i]);
        generations[generationNum++] = gram;//顺序存放产生式
    }
}

void genFirst() {//生成first集
    bool change = true;//作为first集是否还增大的标志
    int i = 0;
    //非终结符号求first集
    while (change) {//只要first集还增大，就循环
        change = false;//初始化为false
        for (set<char>::iterator it = nonTerminate.begin(); it != nonTerminate.end(); ++it) {//遍历非终结符号集，找出产生式
            set<string> generations = grammar[*it];//通过非终结符号找到其所有产生式右部
            for (set<string>::iterator vit = generations.begin(); vit != generations.end(); ++vit) {//遍历这个非终结符号的所有产生式
                string right = *vit;//产生式右部
                int rightlen = right.size();
                char left = *it;//产生式左部
                if (Terminate.find(right[0]) != Terminate.end()) {// x -> aa, 首字符属于终结符号集
                    if (first[left].find(right[0]) == first[left].end()) {//如果不在first集才加入
                        first[left].insert(right[0]);//将首字母加入follow(left)
                        change = true;
                    }
                }
                else if (nonTerminate.find(right[0]) != nonTerminate.end()) {//x -> y1y2...yk, 首字符属于非终结符号集
                    i = 0;
                    for (; i < rightlen; ++i) {//遍历右部的每个字符
                        if (Terminate.find(right[i]) != Terminate.end()) {//属于终结符号
                            if (first[left].find(right[i]) == first[left].end()) {//如果不在first集才加入
                                first[left].insert(right[i]);//将首字母加入first(left)
                                change = true;
                            }
                            break;//遇到终结符号就退出
                        }
                        else {
                            bool forward = false;//若当前非终结符号的first集没有epsilon，则停止向后扫描
                            if (first[right[i]].find(EPSILON) != first[right[i]].end())
                                forward = true;
                            for (set<char>::iterator iter = first[right[i]].begin(); iter != first[right[i]].end(); ++iter) {
                                if (first[left].find(*iter) == first[left].end() && *iter != EPSILON) {//如果不在first集并且不为epsilon才加入
                                    first[left].insert(*iter);//将首字母加入first(left)
                                    change = true;
                                }
                            }
                            if (!forward)
                                break;//停止向后扫描
                        }
                    }
                    if (i == rightlen)//此时y1y2...yk的first集均含有epsilon
                        first[left].insert(EPSILON);//向left的first集加入epsilon
                }
            }
        }
    }
    //终结符号求first集
    for (set<char>::iterator it = Terminate.begin(); it != Terminate.end(); ++it)
        first[*it].insert(*it);
}

void genFollow() {//生成follow集
    bool change = true;//作为first集是否还增大的标志

    follow[BEGIN].insert('#');//规定文法的开始符号是S
    while (change) {
        change = false;//初始化为false
        for (set<char>::iterator it = nonTerminate.begin(); it != nonTerminate.end(); ++it) {//遍历非终结符号集，找出产生式
            set<string> generations = grammar[*it];//通过非终结符号找到其所有产生式右部
            for (set<string>::iterator vit = generations.begin(); vit != generations.end(); ++vit) {//遍历这个非终结符号的所有产生式
                string right = *vit;//产生式右部
                int rightlen = right.size();
                char left = *it;//产生式左部
                for (int i = 0; i < rightlen; i++) {
                    if (i < rightlen - 1 && nonTerminate.find(right[i]) != nonTerminate.end()) {//A -> aB...
                        for (set<char>::iterator iter = first[right[i + 1]].begin(); iter != first[right[i + 1]].end(); ++iter) {
                            if (follow[right[i]].find(*iter) == follow[right[i]].end() && *iter != EPSILON) {//如果不在follow集并且不为epsilon才加入
                                follow[right[i]].insert(*iter);
                                change = true;
                            }
                        }
                        if (first[right[i + 1]].find('$') != first[right[i + 1]].end()) {//$在right[i+1]的first集
                            for (set<char>::iterator iter = follow[left].begin(); iter != follow[left].end(); ++iter) {
                                if (follow[right[i]].find(*iter) == follow[right[i]].end()) {//如果不在follow集才加入
                                    follow[right[i]].insert(*iter);
                                    change = true;
                                }
                            }
                        }
                    }
                    else if (i == rightlen - 1 && nonTerminate.find(right[i]) != nonTerminate.end()) {//A -> aB
                        for (set<char>::iterator iter = follow[left].begin(); iter != follow[left].end(); ++iter) {
                            if (follow[right[i]].find(*iter) == follow[right[i]].end()) {//如果不在follow集才加入
                                follow[right[i]].insert(*iter);
                                change = true;
                            }
                        }
                    }
                }
            }
        }
    }
}


void showFirst() {//显示非终结符号的first集
    for (Set::iterator it = first.begin(); it != first.end(); ++it) {
        cout << it->first << "的first集为：";
        for (set<char>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
            cout << *vit << " ";
        cout << endl;
    }
}

void showFollow() {//显示follow集
    for (Set::iterator it = follow.begin(); it != follow.end(); ++it) {
        if (nonTerminate.find(it->first) != nonTerminate.end()) {
            cout << it->first << "的follow集为：";
            for (set<char>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
                cout << *vit << " ";
            cout << endl;
        }
    }
}


//根据非终结符号找到所有圆点在最左边的项目
set<stateElem> newXiangMu(set<char> ch) {
    set<stateElem> xiangMus;
    for (set<char>::iterator it = ch.begin(); it != ch.end(); ++it) {
        for (int i = 0; i < generationNum; i++) {
            if (generations[i].left == *it) {
                xiangMus.insert({ false,0,generations[i],i + 1 });
                if (nonTerminate.find(generations[i].right[0]) != nonTerminate.end() && ch.find(generations[i].right[0]) == ch.end()) {
                    ch.insert(generations[i].right[0]);
                    it = ch.begin();
                }
            }
        }
    }
    return xiangMus;
}

//闭包操作：A -> a·Xb属于closure(I),且X为非终结符号,则X -> ·...属于closure(I)
void closure(state& s) {
    //遍历所有项目
    set<char> targetCh;//收集当前项目集所有圆点之后的非终结符号
    for (set<stateElem>::iterator it = s.elem.begin(); it != s.elem.end(); ++it) {
        if (it->finish)//归约项目，跳过
            continue;
        char ch = it->gen.right[it->pos];
        if (Terminate.find(ch) != Terminate.end())//圆点之后的符号为终结符号,跳过
            continue;
        targetCh.insert(ch);
    }
    set<stateElem> xiangMus = newXiangMu(targetCh);
    for (set<stateElem>::iterator it = xiangMus.begin(); it != xiangMus.end(); ++it) {//加入新项目
        s.elem.insert(*it);
    }
    s.hasReduce = false;
    bool judgeShift = true;
    int reduceNum = 0;
    for (set<stateElem>::iterator it = s.elem.begin(); it != s.elem.end(); ++it) {
        judgeShift &= it->finish;//每次judgeShift与finish进行相与操作，若所有finish均为true，
        //则judgeShift为true
        //否则为false
        if (it->finish) {//归约项目，跳过
            s.hasReduce = true;//有归约项目
            reduceNum++;
            continue;
        }
        char ch = it->gen.right[it->pos];
        s.chs.insert(ch);
    }
    if (judgeShift)//无后继项目
        s.canShift = false;
    else s.canShift = true;//有后继项目
    s.conflict1 = s.canShift & s.hasReduce ;//二者均为true
    s.conflict2 = reduceNum > 1 ? true : false; //或 reduceNum > 1时，conflict为true
}

//转移操作 I1 = closure(I0, ch)
state go(const state& s, char ch) {
    //1. 将转移的初步项目集归入newState中; 2. 对newState进行closure操作
    state newState;
    //遍历s的所有项目
    for (set<stateElem>::iterator it = s.elem.begin(); it != s.elem.end(); ++it) {
        if (it->finish)//当前项目为归约项目，跳过
            continue;
        if (it->gen.right[it->pos] == ch) {//当前项目的圆点后的符号为ch
            stateElem newStateElem;
            newStateElem = *it;
            newStateElem.pos++;//圆点后移一位
            if (newStateElem.pos == newStateElem.gen.right.size())
                newStateElem.finish = true;
            newState.elem.insert(newStateElem);//将转移后的项目加入新项目集
        }
    }
    closure(newState);//对新项目集进行闭包操作
    return newState;
}

//NODE generations[MAX_NUM];//顺序存放产生式，便于后续填表，下标为产生式编号
//SLR_table table;//分析表
//state states[MAX_NUM];//项目集合,下标为项目集编号
//DFA stateDFA;//识别活前缀的DFA

void genDFA() {//生成DFA
    //首先构造初态集
    stateNum++;
    states[0].number = 0;
    stateElem firstElem = { false,0,generations[0],1 };//第一个项目：S -> ·V=E
    states[0].elem.insert(firstElem);
    closure(states[0]);//闭包操作，至此以后初态项目集构造完成；在求closure时，项目集面临的所有符号应已经保存在chs中
    for (int i = 0; i < stateNum; i++) {
        if (states[i].canShift == false)//首先判断当前项目集是否有后继项目
            continue;//没有就跳到下一个
        for (set<char>::iterator it = states[i].chs.begin(); it != states[i].chs.end(); ++it) {//有就进行转移操作
            bool canPut = true;//是否有新项目集产生，true表示有
            state newState = go(states[i], *it);//转移操作,生成新的项目集
            //判重
            for (int k = 0; k < stateNum; k++) {
                if (newState == states[k]) {//此时意味着states[i] --> states[k](通过*it)
                    Query q = { i,*it };
                    stateDFA[q] = k;//构造DFA，状态i通过*it转换到状态k
                    canPut = false;
                    break;
                }
            }
            if (!canPut)//无新项目集产生，进行下一轮转移操作
                continue;
            newState.number = stateNum;
            states[stateNum++] = newState;
            Query q = { i,*it };
            stateDFA[q] = newState.number;//构造DFA，状态i通过*it转换到状态stateNum-1(最新状态)
        }
    }
}


void showStateElem(const stateElem& s) {
    if (s.finish) {
        cout << s.gen.left << " -> " << s.gen.right;
        cout << "·" << endl;
    }
    else {
        int len = s.gen.right.size();
        cout << s.gen.left << " -> ";
        for (int i = 0; i < len; i++) {
            if (i == s.pos)
                cout << "·";
            cout << s.gen.right[i];
        }
        cout << endl;
    }
}

void showStates() {
    for (int i = 0; i < stateNum; i++) {
        cout << i << "号项目集:" << endl;
        for (set<stateElem>::iterator it = states[i].elem.begin(); it != states[i].elem.end(); ++it) {
            showStateElem(*it);
        }
    }
}

//判断两个集合是否有交集
bool hasCommonElem(const set<char>& t1, const set<char>& t2) {
    for (set<char>::const_iterator it = t1.begin(); it != t1.end(); ++it) {//每次取出t1的一个元素在t2中找，若找到则有交集
        if (t2.find(*it) != t2.end())//找到了
            return true;
    }
    return false;//遍历完t1的所有元素也没有在t2中找到，说明无交集
}

//判断冲突能否解决
bool canSolved(const vector<set<char>>& t) {
    int len = t.size();
    for (int i = 0; i < len; ++i)
        for (int j = i + 1; j < len; ++j)
            if (hasCommonElem(t[i], t[j]))//两个集合有交集
                return false;
    return true;
}

//生成SLR分析表，若能生成则返回true，否则返回false
bool genSLRTable() {//生成SLR分析表,返回true表示此文法为SLR(1)文法
    for (int i = 0; i < stateNum; i++) {//遍历项目集
        bool conflict = states[i].conflict1 || states[i].conflict2;
        if (conflict) {//首先判断冲突是否可解决
            vector<set<char>> temp;//用于存放冲突项目的follow集和圆点之后的元素集合
            set<char> shiftSet;//圆点之后的元素集合
            for (set<stateElem>::iterator it = states[i].elem.begin(); it != states[i].elem.end(); ++it) {//遍历项目集的所有项目
                if (it->finish)//归约项目
                    temp.push_back(follow[it->gen.left]);//将产生式左部的follow集加入
                else shiftSet.insert(it->gen.right[it->pos]);//移进项目，将圆点后的符号集合起来
            }
            temp.push_back(shiftSet);//此时temp中存放所有归约项目左部的follow集和移进项目的圆点之后的符号
            if (!canSolved(temp)) {//不能解决冲突，返回false
                cout << "冲突项目集" << i << "不能解决冲突!" << endl;
                return false;
            }
        }
        //可以解决冲突
        for (set<stateElem>::iterator it = states[i].elem.begin(); it != states[i].elem.end(); ++it) {//遍历项目集的所有项目
            if (it->finish) {//归约项目
                if (it->gen.left == BEGIN && it->pos == it->gen.right.size()) {//接受项目
                    Query q = { i,'#' };
                    slr s = { 'a',-1 };
                    table[q] = s;
                }
                else {//A -> ...·，对所有follow(A)中符号a, 置action[i,a] = rj
                    for (set<char>::iterator vit = follow[it->gen.left].begin(); vit != follow[it->gen.left].end(); ++vit) {
                        Query q = { i,*vit };//当前状态为i,面临符号为*vit
                        slr s = { 'r',it->No};//用it->No+1号产生式归约
                        table[q] = s;
                    }
                }
            }
            else {//移进项目
                char ch = it->gen.right[it->pos];//圆点后的符号
                Query q = { i,ch };//当前状态为i,面临符号为ch
                slr s = { 's',stateDFA[q] };//stateDFA[q]为转移的状态数
                table[q] = s;
            }
        }
    }
    return true;
}

void showGenerations() {//显示产生式
    for (int i = 0; i < generationNum; i++)
        cout << i + 1 << "号产生式：" << generations[i].left << " -> " << generations[i].right << endl;
}

void showSLRTable() {//显示SLR分析表
    cout << "SLR(1)分析表如下所示:" << endl;
    cout << "\t\t\tACTION\t\t\t\t\t\t\t\tGOTO" << endl;
    cout << "\t";
    for (set<char>::iterator it = Terminate.begin(); it != Terminate.end(); ++it)
        cout << *it << "\t";
    cout << '#' << "\t";
    for (set<char>::iterator it = nonTerminate.begin(); it != nonTerminate.end(); ++it) {
        cout << *it << "\t";
    }
    cout << endl;
    for (int i = 0; i < stateNum; i++) {
        cout << i << "\t";
        for (set<char>::iterator it = Terminate.begin(); it != Terminate.end(); ++it) {//终结符号
            Query q = { i,*it };
            if (table.find(q) != table.end()) {
                cout << table[q].kind << table[q].num;
            }
            cout << "\t";
        }
        Query q = { i,'#' };
        if (table.find(q) != table.end()) {
            if (table[q].kind == 'a')
                cout << "acc";
            else
                cout << table[q].kind << table[q].num;
        }
        cout << "\t";
        for (set<char>::iterator it = nonTerminate.begin(); it != nonTerminate.end(); ++it) {//非终结符号
            Query q = { i,*it };
            if (table.find(q) != table.end()) {
                cout << table[q].num;
            }
            cout << "\t";
        }
        cout << endl;
    }
}

//打印状态栈
void printStateStack() {
    int len = state_stack.size();
    for (int i = 0; i < len; i++)
        cout << state_stack[i] << " ";
}

//打印字符栈
void printChStack() {
    int len = ch_stack.size();
    for (int i = 0; i < len; i++)
        cout << ch_stack[i];
}

char newTemp()
{
    char c = 'A';
    result_count++;
    return c + result_count - 1;
}


//语义分析
void semantic(char reduceCh, string juBing) {
    int len = juBing.size();
    if (len == 1) {
        if (Terminate.find(juBing[0]) != Terminate.end()) {//句柄是终结符号
            symbol_table[reduceCh].push_back(juBing[0]);
        }
        else {//句柄为非终结符号
            symbol_table[reduceCh].push_back(symbol_table[juBing[0]].back());
            symbol_table[juBing[0]].pop_back();
        }
    }
    else {
        if (juBing[0] == '(') {// F →(E)
            symbol_table[reduceCh].push_back(symbol_table[juBing[1]].back());
            symbol_table[juBing[1]].pop_back();
        }
        else {//以下情况需要产生四元式
            if (juBing[1] == '=') {//A →V=E
                siYuanShi siyuanshi = { '=',symbol_table[juBing[2]].back(),'_',symbol_table[juBing[0]].back() };
                symbol_table[juBing[2]].pop_back();
                symbol_table[juBing[0]].pop_back();
                siYuanShis.push_back(siyuanshi);
            }
            else {// +、-、*、/
                char arg1 = symbol_table[juBing[0]].back();
                symbol_table[juBing[0]].pop_back();
                char arg2 = symbol_table[juBing[2]].back();
                symbol_table[juBing[2]].pop_back();
                symbol_table[reduceCh].push_back(newTemp());//临时变量
                siYuanShi siyuanshi = { juBing[1],arg1,arg2,symbol_table[reduceCh].back() };
                siYuanShis.push_back(siyuanshi);
            }
        }
    }
}

bool SLR(FILE* fp) {//SLR分析
    char inputCh;//输入符号

    state_stack.push_back(0);
    ch_stack.push_back('#');
    current = getCurrent(fp);
    int procedure = 1;
    cout << "步骤\t符号栈\t输入符号\t分析动作\t下一状态\t状态栈\n";
    while (!feof(fp)) {
        if (current.id == -1)
            break;
        if (current.id == ID) {
            inputCh = current.word[0];
            if (inputCh != 'i') {
                cout << "出现未定义标识符!" << endl;
                return false;
            }

        }
        else inputCh = convertion[current.id][0];
        cout << procedure++ << "\t";
        Query q = { state_stack.back(),inputCh };
        if (table.find(q) == table.end()) {
            cout << "SLR(1)分析失败" << endl;
            return false;
        }
        if (table[q].kind == 's') {//转移
            printStateStack();
            cout << "\t";
            printChStack();
            cout << "\t";
            cout << inputCh << "\t";
            cout << table[q].kind << table[q].num << "\t";
            cout << table[q].num << endl;

            ch_stack.push_back(inputCh);//将输入符号入栈
            state_stack.push_back(table[q].num);//将转移状态入栈
            current = getCurrent(fp);//输入串后移一位
        }
        else if (table[q].kind == 'r') {//归约
            printStateStack();
            cout << "\t";
            printChStack();
            cout << "\t";
            cout << inputCh << "\t";
            cout << table[q].kind << table[q].num << "\t";
            char reduceCh = generations[table[q].num - 1].left;//归约符号
            int reduceNum = generations[table[q].num - 1].right.size();//句柄的长度
            semantic(reduceCh,generations[table[q].num - 1].right);//产生语义动作
            //下面首先同步将符号栈与状态栈的内容出栈(长度为句柄的长度), 然后以状态栈顶的状态和归约符号查询SLR分析表，将新状态入栈
            while (reduceNum) {
                state_stack.pop_back();
                ch_stack.pop_back();
                reduceNum--;
            }
            ch_stack.push_back(reduceCh);//将归约符号入栈
            Query q = { state_stack.back(),reduceCh };
            cout << "GOTO[" << state_stack.back() << ',' << reduceCh << "]=" << table[q].num <<endl;

            state_stack.push_back(table[q].num);//将新状态入栈
        }
        else if (table[q].kind == 'a') {
            printStateStack();
            cout << "\t";
            printChStack();
            cout << "\t";
            cout << inputCh << "\t";
            cout << "acc" << endl;
            return true;
        }
    }
    return false;
}

//显示四元式
void showSiYuanShi() {
    int len = siYuanShis.size();
    for (int i = 0; i < len; i++)
        cout << '(' << siYuanShis[i].op << ',' << siYuanShis[i].arg1 << ',' << siYuanShis[i].arg2 << ',' << siYuanShis[i].res << ')' << endl;
}
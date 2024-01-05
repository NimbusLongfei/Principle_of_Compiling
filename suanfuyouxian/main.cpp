#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<iostream>
#include<stack>

using namespace std;

#define  _CRT_SECURE_NO_WARNINGS  1

char H[200]; //记录非终结符合
char L[200]; //记录终结符号
int num_ll = 3;//记录文法条数
char G_E[100][100] = {
        "E->E+T|E-T|T",
        "T->T*F|T/F|F",
        "F->(E)|i"
};//文法数组
char frist_vt[100][100];//记录fristvt集内容
char last_vt[100][100];//记录lastvt集内容
char relation_table[100][100];//优先关系表
char shizi[500], like[500];//从文件中读取字符串

void openthefile();//打开并读取词法分析文件
int location_H(char c);//判断当前字符是否属于非终结符号
int location_L(char c);//判断当前字符是否属于终结符号
void add_HL(char ll[100][100]);//添加非终结符号与终结符号
void print_result();//输出结果
int check_the_fristvt(char c,int i);//检查Fristvt集中是否有该元素
int check_the_lastvt(char c,int i);//检查Lastvt集中是否有该元素
void find_the_frist(char ll[100][100]);//添加Fristvt集
void find_the_last(char ll[100][100]);//添加Lastvt集
void analysis();//算法优先文法分析函数

void openthefile() {
    cout << "-------------------------------------------------------" << endl;
    cout << "读取文件可知输入语句：" << endl;
    FILE* fp;
    char buf[1000];
    if ((fp = fopen("E:\\desktop\\result.txt", "r")) != NULL) {
        while (fgets(buf, 1000, fp) != NULL)
        {
            int len = strlen(buf);
            printf("%s \n", buf);
            int flag = 0;

            for (int i = 0; i < len; i++) {
                if (buf[i] == '(' && flag == 0) {
                    flag = 1;//识别符号
                    for (int j = i + 1; j < len; j++) {
                        if (buf[j] == ',') {
                            shizi[strlen(shizi)] = buf[j + 1];
                            if ((buf[j + 1] >= 'a' && buf[j + 1] <= 'z') ||
                                (buf[j + 1] >= 'A' && buf[j + 1] <= 'Z')) {
                                like[strlen(like)] += 'i';
                            }
                            else like[strlen(like)] = buf[j + 1];
                            i = j + 1;
                            break;
                        }
                    }
                }
                else if (flag == 1 && buf[i] == ')') {
                    flag = 0;
                }
            }
        }
    }
    shizi[strlen(shizi)] = '#';
    like[strlen(like)] = '#';
    fclose(fp);
    cout << "-------------------------------------------------------" << endl;
    cout << "输入的语句为：" << shizi << endl;
    cout << "将其转化为：" << like << endl;
    cout << "-------------------------------------------------------" << endl;
}

void add_HL(char ll[100][100]) {
    int H_index = 0;
    int L_index = 0;
    int temp = 0;
    int temp1 = 0;
    for (int i = 0; i < num_ll; i++) {//添加非终结符号
        H[H_index] = ll[i][0];
        H_index++;
    }
    for (int i = 0; i < num_ll; i++) {
        for (int j = 3; j < strlen(ll[i]); j++) {
            //由于文法开始前三个字符必定不为终结符号，所以从三号索引开始
            temp = 0;
            for (int k = 0; k < strlen(H); k++) {
                if (ll[i][j] == H[k] || ll[i][j] == '|' || ll[i][j] == 'N') {
                    temp = 1;
                    break;
                }
            }
            if (temp == 0) {
                temp1 = 0;
                for (int k = 0; k < strlen(L); k++) {
                    //判断该非终结符号是否已经存在
                    if (ll[i][j] == L[k]) {
                        temp1 = 1;
                        break;
                    }
                }
                if (temp1 == 0) {
                    L[L_index] = ll[i][j];//添加终结符号
                    L_index++;
                }
            }
        }
    }
    L[strlen(L)] = '#';
}

int location_H(char c) {
    int location = -1;
    for (int i = 0; i < num_ll; i++) {
        if (c == H[i]) {
            location = i;
            break;
        }
    }
    return location;
}
int location_L(char c) {
    int location = -1;
    for (int i = 0; i < strlen(L); i++) {
        if (c == L[i]) {
            location = i;
            break;
        }
    }
    return location;
}
int check_the_fristvt(char c, int i) {
    int temp = -1;
    for (int j = 0; j < strlen(frist_vt[i]); j++) {
        if (frist_vt[i][j] == c) {
            temp = j;
            break;
        }
    }
    return temp;
}
int check_the_lastvt(char c, int i) {
    int temp = -1;
    for (int j = 0; j < strlen(last_vt[i]); j++) {
        if (last_vt[i][j] == c) {
            temp = j;
            break;
        }
    }
    return temp;
}
void print_result() {
    cout << "------------------20231048 马云霄----------------------" << endl;
    cout << "该文法的非终结符号集为：" << H << endl;
    cout << "该文法的终结符号集为：" << L << endl;
    cout << "-------------------------------------------------------" << endl;
    cout << "当前文法的Fristvt集为：" << endl;
    for (int i = 0; i < num_ll; i++) {
        cout <<H[i]<<": "<< frist_vt[i] << endl;
    }
    cout << "当前文法的Lastvt集为：" << endl;
    for (int i = 0; i < num_ll; i++) {
        cout << H[i] << ": " << last_vt[i] << endl;
    }
    cout << "-------------------------------------------------------" << endl;
    cout << "当前优先关系矩阵为：" << endl;
    cout << " ";
    for (int i = 0; i < strlen(L); i++) {
        cout << "  " << L[i];
    }
    cout << endl;
    for (int i = 0; i < strlen(L); i++) {
        cout << L[i];
        for (int j = 0; j < strlen(L); j++) {
            cout << "  " << relation_table[i][j];
        }
        cout << endl;
    }
}
void find_the_frist(char ll[100][100]) {
    int temp;
    for (int i = 0; i < num_ll; i++) {
        temp = 1;
        for (int j = 3; j < strlen(ll[i]); j++) {
            if (temp == 1) {
                if (location_L(ll[i][j]) != -1) {
                    //当前文法的第一个字符为终结符号
                    if (check_the_fristvt(ll[i][j], i) == -1) {
                        //如果该fristvt集没有该符号
                        frist_vt[i][strlen(frist_vt[i])] = ll[i][j];
                        //添加该非终结符号到Fristvt集
                    }
                }
                else {
                    //当前文法的第一个符号不是终结符号
                    if (j != strlen(ll[i]) - 1 && ll[i][j + 1] != '|') {
                        //如果当前文法还没结束且不是单产生式子
                        if (check_the_fristvt(ll[i][j + 1], i) == -1) {
                            frist_vt[i][strlen(frist_vt[i])] = ll[i][j + 1];
                        }
                    }
                }
            }
            temp = 0;
            if (ll[i][j] == '|') {
                temp = 1;//frist集只判断或符号的后面的句子即可
            }
        }
    }

    for (int w = 0; w < 2; w++) {
        //为保证这里获取的Fristvt集完全，该部分遍历两遍程序
        for (int i = num_ll - 1; i >= 0; i--) {
            //反向遍历按照第二条规则添加fristvt集
            temp = 1;
            for (int j = 3; j < strlen(ll[i]); j++) {
                if (temp == 1) {
                    if (location_H(ll[i][j]) != -1) {
                        //文法当前符号为非终结符号
                        for (int k = 0; k < strlen(frist_vt[location_H(ll[i][j])]); k++) {
                            if (check_the_fristvt(frist_vt[location_H(ll[i][j])][k], i) == -1) {
                                //如果该fristvt集没有该符号
                                frist_vt[i][strlen(frist_vt[i])] = frist_vt[location_H(ll[i][j])][k];
                                //添加该非终结符号到Fristvt集
                            }
                        }
                    }
                }
                temp = 0;
                if (ll[i][j] == '|') {
                    temp = 1;
                }
            }
        }
    }
}
void find_the_last(char ll[100][100]) {
    for (int i = 0; i < num_ll; i++) {
        for (int j = 3; j < strlen(ll[i]); j++) {
            if (j==strlen(ll[i])-1 || ll[i][j+1]=='|') {
                if (location_L(ll[i][j]) != -1) {
                    //当前文法的字符为终结符号
                    if (check_the_lastvt(ll[i][j], i) == -1) {
                        //如果该lastvt集没有该符号
                        last_vt[i][strlen(last_vt[i])] = ll[i][j];
                        //添加该非终结符号到lastvt集
                    }
                }
                else {
                    //当前文法的符号不是终结符号
                    if (location_L(ll[i][j-1])!=-1) {
                        //该非终结符号前面是终结符号
                        if (check_the_lastvt(ll[i][j-1], i) == -1) {
                            last_vt[i][strlen(last_vt[i])] = ll[i][j - 1];
                        }
                    }
                }
            }
        }
    }
    for (int w = 0; w < 2; w++) {
        //为保证这里获取的Fristvt集完全，该部分遍历两遍程序
        for (int i = num_ll - 1; i >= 0; i--) {
            //反向遍历按照第二条规则添加fristvt集
            for (int j = 3; j < strlen(ll[i]); j++) {
                if (j == strlen(ll[i])-1 || ll[i][j + 1] == '|') {
                    if (location_H(ll[i][j]) != -1) {
                        //文法当前符号为非终结符号
                        for (int k = 0; k < strlen(last_vt[location_H(ll[i][j])]); k++) {
                            if (check_the_lastvt(last_vt[location_H(ll[i][j])][k], i) == -1) {
                                //如果该fristvt集没有该符号
                                last_vt[i][strlen(last_vt[i])] = last_vt[location_H(ll[i][j])][k];
                                //添加该非终结符号到Fristvt集
                            }
                        }
                    }
                }
            }
        }
    }
}
void make_relation_table(char ll_1[100][100]) {
    char ll[100][100];
    //为该文法添加开始符号与结束符号
    for (int i = 1; i < num_ll + 1; i++) {
        strcpy(ll[i], ll_1[i - 1]);
    }
    strcpy(ll[0],"S->#E#");
    //优先关系矩阵的初始化
    for (int i = 0; i < strlen(L); i++) {
        for (int j = 0; j < strlen(L); j++) {
            relation_table[i][j] = 'N';
        }
    }
    for (int i = 0; i < num_ll+1; i++) {
        for (int j = 3; j < strlen(ll[i]); j++) {
            //添加等于关系
            if (location_H(ll[i][j]) == -1 && ll[i][j] != '|') {
                //该符号为终结符号
                if (j < strlen(ll[i]) - 1) {
                    if (location_H(ll[i][j + 1]) == -1 && ll[i][j] != '|') {
                        //如果两个终结符号相连,优先关系相等
                        relation_table[location_L(ll[i][j])][location_L(ll[i][j + 1])] = '=';
                    }
                }
                if (j < strlen(ll[i]) - 2) {
                    if (location_H(ll[i][j + 1]) != -1 && ll[i][j + 2] != '|' && location_H(ll[i][j + 2]) == -1) {
                        //如果两个终结符号中间有一个非终结符，优先关系相等
                        relation_table[location_L(ll[i][j])][location_L(ll[i][j + 2])] = '=';
                    }
                }
            }
            if (j < strlen(ll[i]) - 1) {
                //添加低于优先关系
                if (location_H(ll[i][j]) == -1 && location_H(ll[i][j+1])!=-1 && ll[i][j]!='|') {
                    //当前字符为终结符号且后面跟着非终结符号
                    for (int k = 0; k < strlen(frist_vt[location_H(ll[i][j + 1])]); k++) {
                        relation_table[location_L(ll[i][j])][location_L(frist_vt[location_H(ll[i][j + 1])][k])] = '<';
                    }
                }
                    //添加高于优先关系
                else if (location_H(ll[i][j]) != -1 && location_H(ll[i][j + 1]) == -1 && ll[i][j+1] != '|') {
                    //当前字符为非终结符且后面跟着终结符
                    for (int k = 0; k < strlen(last_vt[location_H(ll[i][j])]); k++) {
                        relation_table[location_L(last_vt[location_H(ll[i][j])][k])][location_L(ll[i][j+1])] = '>';
                    }
                }
            }
        }
    }
}
void analysis() {
    cout << "    分析栈" << "           " << "语句" << endl;
    char a1, a2;
    int like_index = 0;
    char stack[100];//分析栈
    char s[100];//输入语句
    int j;
    char q;
    int lengh;
    int temp=1;
    stack[like_index] = '#';
    stack[1] = '\0';
    for (int i = 0; i < strlen(like); i++) {
        temp = 1;
        if (location_H(stack[strlen(stack)-1]) == -1) {
            j = strlen(stack) - 1;
        }
        else j = strlen(stack) - 2;
        //当前关系为等于，入栈
        if (relation_table[location_L(stack[j])][location_L(like[i])] == '=') {
            like_index = strlen(stack);
            stack[like_index]=like[i];
            stack[like_index + 1] = '\0';
        }
            //低于关系也入栈
        else if (relation_table[location_L(stack[j])][location_L(like[i])] == '<') {
            like_index = strlen(stack);
            stack[like_index] = like[i];
            stack[like_index + 1] = '\0';
        }
        else if (relation_table[location_L(stack[j])][location_L(like[i])] == '>') {
            //寻找句柄进行归约
            for (int y=0;;y++) {
                for (;;) {
                    q = stack[j];
                    lengh = j;
                    if (j >= 1 && location_H(stack[j - 1]) == -1) {
                        j--;
                    }
                    else if (j >= 2) {
                        j = j - 2;
                    }
                    if (relation_table[location_L(stack[j])][location_L(q)] == '<') {
                        break;
                    }
                }
                stack[j + 1] = '\0';
                like_index = strlen(stack);
                stack[like_index] = 'E';
                stack[like_index + 1] = '\0';
                if (y == 0) {
                    for (int u = i; u < strlen(like); u++) {
                        s[u - i] = like[u];
                        lengh = u - i+1;
                    }
                    s[lengh] = '\0';
                    cout.width(10);
                    cout.unsetf(ios::left);
                    cout.fill(' ');
                    cout << stack;
                    cout << "         " << s << endl;
                }
                like_index = strlen(stack);
                stack[like_index] = like[i];
                stack[like_index + 1] = '\0';
                for (int u = i; u < strlen(like); u++) {
                    s[u - i] = like[u];
                    lengh = u - i + 1;
                }
                cout.width(10);
                cout.unsetf(ios::left);
                cout.fill(' ');
                s[lengh] = '\0';
                cout << stack;
                cout << "         " << s << endl;
                temp = 0;
                if (strlen(stack) <= 2) { break; }
                a1 = stack[strlen(stack) - 1];
                if (location_H(stack[strlen(stack) - 2]) == -1) {
                    a2 = stack[strlen(stack) - 2];
                }
                else a2 = stack[strlen(stack) - 3];
                if (relation_table[location_L(a2)][location_L(a1)] == '<' || relation_table[location_L(a2)][location_L(a1)] == '=') {
                    break;
                }
            }
        }
        else {
            cout << "分析出错！" << endl;
            break;
        }
        if (temp != 0) {
            for (int u = i; u < strlen(like); u++) {
                s[u - i] = like[u];
                lengh = u - i + 1;
            }
            s[lengh] = '\0';
            cout.width(10);
            cout.unsetf(ios::left);
            cout.fill(' ');
            cout << stack;
            cout<< "         " << s << endl;
        }
    }
    if (strcmp(stack, "#E#") == 0) {
        cout << "分析成功！" << endl;
    }
}
int main(void) {
    add_HL(G_E);//添加终结符号与非终结符号
    find_the_frist(G_E);//寻找Fristvt集
    find_the_last(G_E);//寻找Lastvt集
    make_relation_table(G_E);//构建优先关系表格
    print_result();//输出结果
    openthefile();//读取词法分析输出文法
    //strcpy(like, "(i+i)*i#");
    analysis();//进行算符优先文法分析
    return 0;
}

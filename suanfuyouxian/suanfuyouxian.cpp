#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
using namespace std;

map<char, vector<string>> rule;
map<char, int> V;
map<char, set<char>> firstvt;
map<char, set<char>> lastvt;
char V_T[1000];
map<pair<char, char>, int> table;
int cnt;
string f, l, seq;
int seqpos = 0, nowtoppos = 0;
char nowtop = '#', nowbot;
int real = -1;

void readwenfa(){
    ifstream inputfile;
    inputfile.open("../wenfa.txt");
    if(inputfile.is_open()){
        string line;
        while(getline(inputfile, line)){
            if (line.length() > 0 && line[ - 1] == '\n') {
                line[line.length() - 1] = '\0';
            }
            size_t arrowPos = line.find("->");
            if (arrowPos != string::npos) {
                string ll = line.substr(arrowPos + 2);
                rule[line[0]].push_back(ll);
                for(int i = 0; i < ll.length(); i++){
                    if(isalpha(ll[i]) && ll[i] - 'A' <= 25) V[ll[i]] = 1;
                    else{
                        V[ll[i]] = -1;
                        V_T[cnt++] = ll[i];
                    }
                }
                if(V[ll[0]] == 1){
                    if(f.find(line[0]) == string::npos) f += line[0];
                    size_t pos = f.find(ll[0]);
                    if(pos == string::npos){
                        f.insert(f.find(line[0]), 1,ll[0]);
                    }
                }

                if(V[ll[ll.length()-1]] == 1){
                    if(l.find(line[0]) == string::npos) l += line[0];
                    size_t pos = l.find(ll[ll.length()-1]);
                    if(pos == string::npos){
                        l.insert(l.find(line[0]), 1, ll[ll.length()-1]);
                    }
                }
            }
        }
    }
    inputfile.close();
    return;
}
void readseq(){
    ifstream inputseq;
    inputseq.open("../seq2.txt");
    if(inputseq.is_open()){
        if(getline(inputseq, seq)){
            seq +="#";
        }
    }
    cout << seq << endl;

    for (int i = 0; i < seq.length(); ++i) {
        if(isalpha(seq[i]) && seq[i] - 'A' > 25) seq[i] = 'i';
    }
    cout << seq << endl;
    return;
}


void calfirst_vt(){
    for(const auto& product :rule){
        char nowVT = product.first;
        for (const auto& right : product.second) {
            if(V[right[0]] == 1 && V[right[1]] == -1){
                firstvt[nowVT].insert(right[1]);
            }else if(V[right[0]] == -1){
                firstvt[nowVT].insert(right[0]);
            }
        }
    }
    for (int i = 0; i < f.length() - 1; ++i) {
        char now_VN = f[i];
        firstvt[f[i+1]].insert(firstvt[now_VN].begin(), firstvt[now_VN].end());
    }
    cout << "----------------------FirstVT--------------------" << endl;
    for(const auto& now : firstvt){
        cout << now.first << " ";
        for (const auto& nowrule : now.second)
            cout << nowrule;
        cout << endl;
    }

    return ;
}

void callast_vt(){
    for(const auto& product :rule){
        char nowVT = product.first;
        for (const auto& right : product.second) {
            if(V[right[right.length() -1]] == 1 && V[right[right.length()-2]] == -1){
                lastvt[nowVT].insert(right[right.length()-2]);
            }else if(V[right[right.length()-1]] == -1){
                lastvt[nowVT].insert(right[right.length()-1]);
            }
        }
    }
    for (int i = 0; i < l.length() - 1; ++i) {
        char now_VN = l[i];
        lastvt[l[i+1]].insert(lastvt[now_VN].begin(), lastvt[now_VN].end());
    }
    cout << "----------------------LastVT--------------------" << endl;
    for(const auto& now : lastvt){
        cout << now.first << " ";
        for (const auto& nowrule : now.second)
            cout << nowrule;
        cout << endl;
    }
    return ;
}

void fenxitable(){
    //添加规则
    vector<string> a = rule['E'];
    a.push_back("#E#");
    rule['E'] = a;
    V_T[cnt++] = '#';
    V['#'] = -1;

    for (const auto& now : rule) {
        char nowvn = now.first;
        for (const auto& right : now.second) {
            for(int i = 0; i < right.length() - 1; i++){
                if(V[right[i]] == -1 && V[right[i+1]] == 1){
                    for(const auto& firstvtnow : firstvt[right[i+1]]){
                        table[{right[i],firstvtnow}] = -1;
                    }
                }
                if (V[right[i]] == 1 && V[right[i+1]] == -1){
                    for(const auto& lastvtnow : lastvt[right[i]]){
                        table[{ lastvtnow, right[i+1]}] = 1;
                    }
                }
                if(i + 2 < right.length()){
                    if(V[right[i]] == -1 && V[right[i+2]] == -1){
                        table[{right[i], right[i+2]}] = 2;
                    }
                }else{
                    if(V[right[i]] == -1 && V[right[i+1]] == -1){
                        table[{right[i], right[i+1]}] = 2;
                    }
                }
            }
        }
    }
    cout << "--------------------------table--------------------------"<<endl;
    cout << "  ";
    for (int i = 0; i < cnt; ++i) {
        cout << V_T[i] << "  ";
    }
    cout << endl;
    for(int k = 0; k < cnt; k++){
        for(int m = 0; m < cnt; m++){
            if(m == 0) cout << V_T[k];
            if(table[{V_T[k], V_T[m]}] == 2) cout << " = ";
            else if(table[{V_T[k], V_T[m]}] == 1) cout << " > ";
            else if(table[{V_T[k], V_T[m]}] == -1) cout << " < ";
            else cout << "   ";
        }
        cout << endl;
    }

    return;
}

vector<char> stack;

void printStack() {
    cout << "Stack: ";
    for (const auto& symbol : stack) {
        cout << symbol << " ";
    }
    cout << "       ";
    for(int j = seqpos; j < seq.length(); j++){
        cout << seq[j];
    }
    cout << "   ";
}

void parse(){
    stack.push_back('#');
    vector<string> newright = {"S+S", "S-S", "S*S", "S/S","(S)","#S#", "i"};
//    map<char, vector<string>> newrule;
//    newrule['S'] = newright;
    V['S'] = 1;

    for(int i = 0;  ; i++){
        if(table[{nowtop, seq[seqpos]}] == -1){
            printStack();
            cout << endl;
            nowbot = nowtop;
            nowtop = seq[seqpos];
            stack.push_back(seq[seqpos]);
            seqpos++;
            for (int j = nowtoppos+1; j < stack.size(); ++j) {
                if(V[stack[j]] == -1){
                    nowtoppos = j;
                    if(nowtop != stack[nowtoppos]){
                        cout<< "error <" <<endl;
                        break;
                    }
                }
            }
        }else if (table[{nowtop, seq[seqpos]}] == 2){
            printStack();
            cout << endl;
            nowtop = seq[seqpos];
            stack.push_back(seq[seqpos]);
            seqpos++;
        }else if (table[{nowtop, seq[seqpos]}] == 1){
            printStack();
            string suduanyu = "";
            for(; ; stack.pop_back()){
                int n = stack.size();
                if(stack.back() == nowbot ){
                        break;
                }
                suduanyu.insert(suduanyu.begin(), stack.back());
            }
//            cout <<"nowbot : " << nowbot << endl;
//            if(nowbot == '('){
//                if(stack.size() != real + 1) {
//                    suduanyu.insert(suduanyu.begin(), stack.back());
//                    stack.pop_back();
//                }
//            }
                if(suduanyu.back() == ')' && real != -1){
                    suduanyu.insert(suduanyu.begin(), stack.back());
                    stack.pop_back();
                }

            cout << "suduanyu :" << suduanyu << endl;
//            cout <<"nowbot : " << nowbot << endl;

            for(const auto& nowrule : rule){
                for(const auto& nowright : nowrule.second){
                    if(nowright == suduanyu) {
                        stack.push_back(nowrule.first);
                    }
                }
            }
            real = -1;
            nowtop = nowbot;
            bool flag = false;
            for(int i = stack.size() - 1; i >= 0 ;i--){
                if(V[stack[i]] == -1){
                    for(int j = i-1; j >=0; j--){
                        if(V[stack[j]] == -1 && table[{stack[j], stack[i]}] == -1){
                            if(stack[j] == stack[i]) real = j;
//                            cout << stack[i] << stack[j] << table[{stack[j], stack[i]}]<<endl;
                            nowbot = stack[j];
//                            cout << nowbot<< endl;
                            flag = true;
                            break;
                        }
                    }
                }
                if(flag) break;
            }
            if(!flag) nowbot = nowtop;
//            cout << "new nowbot:" << nowbot << endl;
        }
        vector<char> target = {'#','E','#'};
        bool ans = true;
        if(seqpos >= seq.length()){
            for(size_t i = 0; i < stack.size(); i++){
                if(stack[i] != target[i]){
                    ans = false;
                }
            }
            if(ans){
                cout<<"right"<<endl;
                break;
            }
            else{
                cout << "error" << endl;
                break;
            }
        }
    }
    return;
}
int main(void){
    f = "";
    l = "";
    readwenfa();
    calfirst_vt();
    callast_vt();
    fenxitable();
    readseq();
    parse();


    return 0;
}
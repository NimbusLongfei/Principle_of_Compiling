#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

struct Data {
    int number;
    string text;
};
int j = 0;
bool f = false;


Data parseLine(const string &line) {
    Data data;
    istringstream iss(line);
    char openParen, comma, closeParen;
    iss >> openParen >> data.number >> comma >> data.text;
    size_t lastRightParenPos = data.text.rfind(')');
    data.text = data.text.substr(0, lastRightParenPos);
    return data;
}

void S(Data(&data)[100]);//
void E(Data(&data)[100]);//
void E1(Data(&data)[100]);//
bool T(Data(&data)[100]);//
void T1(Data(&data)[100]);//
void F(Data(&data)[100]);//
bool A(Data(&data)[100]);//
bool M(Data(&data)[100]);//
void V(Data(&data)[100]);//



void S(Data(&data)[100]) {
    if (!f) {
        cout << "S  ";
        if (data[j].text == "i") {
            V(data);
            if (!f && data[j].text == "=") {
                j++;
                E(data);
            } else {
                f = true;
                cout << "\nS false";
                return;
            }
        } else {
            f = true;
            cout << "\nS false";
            return;
        }
    }
}

void V(Data(&data)[100]) {
    if (!f) {
        cout << "V  ";
        if (data[j].text == "i") j++;
        else {
            f = true;
            cout << "\nV false";
        }
    }
    return;
}

void E(Data(&data)[100]) {
    if (!f) {
        cout << "E  ";
        if (data[j].text == "(" || data[j].text == "i") {
            T(data);
            if (!f) {
                if(data[j].text == "+" || data[j].text == "-"){
                    E1(data);
                }else if(data[j].text == ")" || data[j].text == "#"){
                    return;
                }else{
                    f = true;
                    cout << "\nE false";
                }
            }

        }else{
            f = true;
            cout << "\nE false";
            return;
        }
    }
}

void E1(Data(&data)[100]) {
    if (!f) {
        cout << "E'  ";
        if (data[j].text == "+" || data[j].text == "-") {
            if (A(data)) {
                if (T(data)) {
                    if(data[j].text == "+" || data[j].text == "-"){
                        E1(data);
                    }else if(data[j].text == ")" || data[j].text == "#"){
                        return;
                    }else{
                        f= true;
                        cout << "\nE false";
                    }
                } else {
                    f = true;
                    return;
                }
            } else {
                f = true;
                return;
            }
        } else if (data[j].text == ")" || data[j].text == "#") {
            return;
        } else {
            f = true;
            cout << "\n E1 false";
            return;
        }
    }
}

bool T(Data(&data)[100]) {
    if (!f) {
        cout << "T  ";
        if (data[j].text == "(" || data[j].text == "i") {
            F(data);
            if (!f) {
                T1(data);
                if (!f) {
                    return true;
                } else {
                    f = true;
                    cout<<"\nT false";
                    return false;
                }
            } else {
                f = true;
                cout << "\nT false";
                return false;
            }
        } else {
            f = true;
            cout << "\nT false";
            return false;
        }
    }
}

void T1(Data(&data)[100]) {
    if (!f) {
        cout << "T'  ";
        if (data[j].text == "*" || data[j].text == "/") {
            if (M(data)) {
                F(data);
                if (!f) {
                    T1(data);
                }
            } else {
                f = true;
                return;
            }
        } else if (data[j].text == "+" || data[j].text == "-" || data[j].text == ")" || data[j].text == "#") {
            return;
        } else {
//            cout << data[10].text << endl;
            f = true;
            cout << "\nT' false";
            return;
        }
    }
}

bool A(Data(&data)[100]) {
    if (!f) {
        cout << "A  ";
        if (data[j].text == "+" || data[j].text == "-") {
            j++;
            return true;
        } else {
            f = false;
            cout << "\nA false";
            return false;

        }
    }
}

bool M(Data(&data)[100]) {
    if (!f) {
        cout << "M  ";
        if (data[j].text == "*" || data[j].text == "/") {
            j++;
            return true;
        } else {
            f = true;
            cout << "\nM false";
            return false;
        }
    }
}

void F(Data(&data)[100]) {
    if (!f) {
        cout << "F   ";
        if (data[j].text == "(") {
            j++;
            if (data[j].text == "(" || data[j].text == "i"){
                E(data);
                if (!f) {
                    if (data[j].text == ")") j++;
                    else{
                        f = true;
                        cout << "\nF false";
                        return;
                    }
                } else{
                    f = true;
                    cout << "\nF false";
                    return;
                }
            }else{
                f = true;
                cout << "\nF false";
            }
        } else if (data[j].text == "i") {
            j++;
            return;
        } else {
            f = true;
            cout << "\nF false";
            return;
        }
    }
    return;
}


int main(void) {
    string input;
    Data begin[100];
    int i = 0;
    while (true) {
        getline(std::cin, input);
        if (input == "end") break;
        begin[i] = parseLine(input);
//        cout << "Number: " << begin[i].number << ", Text: " << begin[i].text << endl;
        i++;
    }
    begin[i].text = "#";
    begin[i].number = -1;

    for (int l = 0; l < i; l++) {
        for(int n = 0; n < begin[l].text.length();n++){
            if(isalpha(begin[l].text[n])){
                begin[l].text = "i";
                break;
            }
        }
    }
    for(int k = 0; k <= i ; k++) cout << begin[k].text;
    cout << endl;
    S(begin);
    cout << endl;
    if(f) cout << "error"<<endl;
    else cout << "right" <<endl;
    return 0;
}
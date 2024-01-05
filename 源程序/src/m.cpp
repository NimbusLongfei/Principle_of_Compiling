#include <iostream>
#include "SLR.h"
#include "lex.h"
using namespace std;

int main(){
    init();
    string filename = "�ķ�.txt"; // ��ȡ·�� 
    setGrammar(filename);
    genFirst();//����first��
    genFollow();//����follow��
    showFirst();//��ʾfirst��
    showFollow();//��ʾfollow��
    showGenerations(); // �����źõĲ���ʽ 
    genDFA();//����DFA
    showStates();//��ʾ��Ŀ��
    genSLRTable();//����SLR(1)������
    showSLRTable();//��ʾSLR(1)������
    fflush(stdout);
    FILE* fp = fopen("SLR_input.txt", "r");// �ʷ��������·�� 

    if (SLR(fp)) {//����SLR��������
        cout << "���������﷨����" << endl;
        cout << "��Ԫʽ������ʾ��" << endl;
        showSiYuanShi();//��ʾ��Ԫʽ
    }
    else
        cout << "����䲻�����﷨����" << endl;
    
    return 0;

}

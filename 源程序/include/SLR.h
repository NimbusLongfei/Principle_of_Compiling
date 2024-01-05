#include <string>
#include <iostream>
#include <set>
#include <map>
#include <unordered_map>
#include <stack>

using namespace std;

#define BEGIN 'Q'
#define MAX_NUM 100
#define EPSILON '$'

//����ʽ
typedef struct node {
	char left;//����ʽ��
	string right;//����ʽ�Ҳ�

	// ���ص�������� 
	bool operator == (const struct node item) const {
		return left == item.left && right == item.right;
	} 
}NODE;

//ͨ��״̬��������Ӳ�ѯSLR������Ԫ��
typedef struct query {
	int state;//״̬
	char ch;//����
}Query;

//SLR�������Ԫ��
typedef struct slr {
	char kind;//'r': ��Լ�� 's': �ƽ��� 'a': ����; 'e': ����
	int num;//��ʾ�õڼ�������ʽ��Լ���ƽ���״̬��,-1��ʾ������Ŀ��-2��ʾ����
}slr;


//��Ŀ����Ԫ�����ͣ���Լ����λ������ʽ���ڼ����� 
typedef struct stateElem {
	bool finish;//��ʾ�Ƿ�Ϊ��Լ��false��ʾ���ǣ�true��ʾ��
	int pos;//��ʾԲ���λ�ã�ȡֵ��0-gen.size(),��Ϊgen.size()ʱ����ʾ��������ǰԲ�����ٵķ���Ϊ��gen.right[pos]
	NODE gen;//����ʽ
	int No;//��ʾ�ڼ�������ʽ,��1��ʼ

	bool operator==(const stateElem& item)const {//���ں�����
		return gen == item.gen && pos == item.pos;
	}
	bool operator<(const stateElem& item)const {//Ϊ�˶�set��key��������
		return No < item.No || (No == item.No && pos < item.pos);
	}
	void operator=(const stateElem& item) {//��ֵ���������
		finish = item.finish;
		pos = item.pos;
		gen = item.gen;
		No = item.No;
	}
}stateElem;

//��Ŀ��
typedef struct state {
	int number;//��Ŀ���
	//�����ĸ�����closure���� 
	set<stateElem> elem;//��Ŀ
	set<char> chs;//������Ϻ���Ŀ�����ٵ����з��Ŵ���chs�� 
	bool conflict1;//��Լ���ƽ���ͻ
	bool conflict2;//��Լ��ͻ 
	bool canShift;//��ʾ�Ƿ��к����Ŀ
	bool hasReduce;//��ʾ�Ƿ��й�Լʽ
	bool operator==(const state& item)const {
		return elem == item.elem;
	}
}state;

//��Ԫʽ
typedef struct siYuanShi {
	char op;
	char arg1;
	char arg2;
	char res;
}siYuanShi;

//���ű�
typedef map<char, deque<char> > Symbol;

//�º����������˺�������������� 
class compareQuery {
public:
	bool operator()(const Query& a, const Query& b) const {
		return a.state < b.state || (a.state == b.state && a.ch < b.ch);//����
	}
};


//SLR������, ͨ��Query�ҷ�����Ԫ��
typedef map<Query, slr, compareQuery> SLR_table;

//��Ŀ���淶��DFA��ͨ��Query����Ŀ�����
typedef map<Query, int, compareQuery> DFA;

//ͨ�����ս�����ҵ������в���ʽ�Ҳ�
typedef unordered_map<char, set<string> > NODES;

//ͨ�����ս�����ҵ�first���Ϻ�follow����
typedef map<char, set<char> > Set;

typedef struct t{
	int id;
	string word;
}WORD;//�ʷ������Ķ�Ԫ��ṹ��


void setGrammar(const string& filename);//�����ķ�NODES
void genFirst();//����first��
void genFollow();//����follow��
void showFirst();//��ʾfirst��
void showFollow();//��ʾfollow��
void showSLRTable();//��ʾSLR������
void showStates();//��ʾ��Ŀ��
void showGenerations();//��ʾ����ʽ

void genDFA();//����DFA
bool genSLRTable();//����SLR������,����true��ʾ���ķ�ΪSLR(1)�ķ�
bool SLR(FILE* fp);//SLR����
void showSiYuanShi();//��ʾ��Ԫʽ

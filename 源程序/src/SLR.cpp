#include "SLR.h"
#include "lex.h"
#include <vector> 
#include <fstream>

Set first, follow;//first����follow��
NODES grammar;//�ķ�����
NODE generations[MAX_NUM];//˳���Ų���ʽ�����ں�������±�Ϊ����ʽ���
set<char> nonTerminate, Terminate;//���ս���ż�, �ս���ż�
SLR_table table;//������
vector<int> state_stack;//״̬ջ
vector<char> ch_stack;//����ջ
state states[MAX_NUM];//��Ŀ����,�±�Ϊ��Ŀ�����
Symbol symbol_table;//���ű�
vector<siYuanShi> siYuanShis;//��Ԫʽ����
int result_count = 0;//��Ԫʽ����
int stateNum = 0;//��Ŀ������
int generationNum = 0;//����ʽ����
DFA stateDFA;//ʶ���ǰ׺��DFA
WORD current = { -1,"" };
unordered_map<int, string> convertion;

WORD getCurrent(FILE* fp) {
    WORD current;

    current.word.clear();
    char line[256]; 
    if (fgets(line, sizeof(line), fp) != NULL) {

        int id;
        char word[256];

        if (sscanf(line, "(%d,%s)", &id, word) == 2) {
            current.id = id;
            current.word = string(word); 
        } else	cerr << "Error: Invalid input format." << endl;
       
    } else {
        current.id = -1;
    }

    return current;
}


//�ֽ����ʽ��->
NODE splitGeneration(string generation) {
	string right;
	int len = generation.size();
	int pos = 0;
	for (; pos < len; pos++)
		if (generation[pos] == '>') {
			pos++;
			break;//�ҵ�->����׸��ǿ��ַ�
		}
	right = generation.substr(pos, len - pos);
	return {generation[0], right}; // ����grammar �� left �� right 
}


void setGrammar(const string& filename) {//�����ķ�NODES, ͬʱ�ռ����ս�������ս����
	ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        cout << "�޷����ļ�" << endl;
        return;
    }
    cout << "���ļ���ȡ�ķ���" << endl;
    
    
    string generation;
    NODE gram;
    while (getline(inputFile, generation)){
		if(generation == "end") break;
		gram = splitGeneration(generation);//���ȷֽ����ʽ
		grammar[gram.left].insert(gram.right);//�ٽ��˲���ʽ�����������
		nonTerminate.insert(gram.left);//�ռ����ս����
		
		int rightLen = gram.right.size();//����ʽ�Ҳ�����
		// �ս���ż��� 
		for (int i = 0; i < rightLen; i++)
			if (!((gram.right)[i] >= 'A' && (gram.right)[i] <= 'Z'))
				Terminate.insert(gram.right[i]);
		generations[generationNum++] = gram;//˳���Ų���ʽ
	}
}

void genFirst() {
	/*����First����*/
	bool change = true;//��Ϊfirst���Ƿ���������󣩵ı�־
	int i = 0;
	//���ս������first��
	while (change) {//ֻҪfirst�������󣬾�ѭ��
		change = false;//��ʼ��Ϊfalse
		for (set<char>::iterator it = nonTerminate.begin(); it != nonTerminate.end(); ++it) {//�������ս���ż�
			set<string> generations = grammar[*it];//ͨ�����ս�����ҵ������в���ʽ�Ҳ�
			for (set<string>::iterator vit = generations.begin(); vit != generations.end(); ++vit) {//����������ս���ŵ����в���ʽ
				string right = *vit;//����ʽ�Ҳ�
				int rightlen = right.size();
				char left = *it;//����ʽ��
				if (Terminate.find(right[0]) != Terminate.end()) {// x -> a, ���ַ������ս���ż�
					if (first[left].find(right[0]) == first[left].end()) {//�������first���ż���
						first[left].insert(right[0]);//������ĸ����follow(left)
						change = true;
					}
				}
				else if (nonTerminate.find(right[0]) != nonTerminate.end()) {//x ->Y1, ���ַ����ڷ��ս���ż�
					i = 0;
					for (; i < rightlen; ++i) {//�����Ҳ���ÿ���ַ�
						if (Terminate.find(right[i]) != Terminate.end()) {//�����ս����
							if (first[left].find(right[i]) == first[left].end()) {//�������first���ż���
								first[left].insert(right[i]);//������ĸ����first(left)
								change = true;
							}
							break;//�����ս���ž��˳�
						}
						else {
							bool forward = false;//����ǰ���ս���ŵ�first��û��epsilon����ֹͣ���ɨ��
							if (first[right[i]].find(EPSILON) != first[right[i]].end())
								forward = true;
							for (set<char>::iterator iter = first[right[i]].begin(); iter != first[right[i]].end(); ++iter) {
								if (first[left].find(*iter) == first[left].end() && *iter != EPSILON) {//�������first�����Ҳ�Ϊepsilon�ż���
									first[left].insert(*iter);//������ĸ����first(left)
									change = true;
								}
							}
							if (!forward)
								break;//ֹͣ���ɨ��
						}
					}
					if (i == rightlen)//��ʱy1y2...yk��first��������epsilon
						first[left].insert(EPSILON);//��left��first������epsilon
				}
			}
		}
	}
	//�ս������first��
	for (set<char>::iterator it = Terminate.begin(); it != Terminate.end(); ++it)
		first[*it].insert(*it);
}

void genFollow() {//����follow��
	bool change = true;//��Ϊfirst���Ƿ�����ı�־

	follow[BEGIN].insert('#');//�涨�ķ��Ŀ�ʼ������S
	while (change) {
		change = false;//��ʼ��Ϊfalse
		for (set<char>::iterator it = nonTerminate.begin(); it != nonTerminate.end(); ++it) {//�������ս���ż����ҳ�����ʽ
			set<string> generations = grammar[*it];//�ҵ������в���ʽ�Ҳ�
			for (set<string>::iterator vit = generations.begin(); vit != generations.end(); ++vit) {//����������ս���ŵ����в���ʽ
				string right = *vit;//����ʽ�Ҳ�
				int rightlen = right.size();
				char left = *it;//����ʽ��
				for (int i = 0; i < rightlen; i++) {
					if (i < rightlen - 1 && nonTerminate.find(right[i]) != nonTerminate.end()) {//A -> aB...
						for (set<char>::iterator iter = first[right[i + 1]].begin(); iter != first[right[i + 1]].end(); ++iter) {
							if (follow[right[i]].find(*iter) == follow[right[i]].end() && *iter != EPSILON) {//�������follow�����Ҳ�Ϊepsilon�ż���
								follow[right[i]].insert(*iter);
								change = true;
							}
						}
						if (first[right[i + 1]].find('$') != first[right[i + 1]].end()) {//$��right[i+1]��first��
							for (set<char>::iterator iter = follow[left].begin(); iter != follow[left].end(); ++iter) {
								if (follow[right[i]].find(*iter) == follow[right[i]].end()) {//�������follow���ż���
									follow[right[i]].insert(*iter);
									change = true;
								}
							}
						}
					}
					else if (i == rightlen - 1 && nonTerminate.find(right[i]) != nonTerminate.end()) {//A -> aB
						for (set<char>::iterator iter = follow[left].begin(); iter != follow[left].end(); ++iter) {
							if (follow[right[i]].find(*iter) == follow[right[i]].end()) {//�������follow���ż���
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

void showFirst() {//��ʾ���ս���ŵ�first��
	for (Set::iterator it = first.begin(); it != first.end(); ++it) {
		cout << it->first << "��first��Ϊ��";
		for (set<char>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
			cout << *vit << " ";
		cout << endl;
	}
}

void showFollow() {//��ʾfollow��
	for (Set::iterator it = follow.begin(); it != follow.end(); ++it) {
		if (nonTerminate.find(it->first) != nonTerminate.end()) {
			cout << it->first << "��follow��Ϊ��";
			for (set<char>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
				cout << *vit << " ";
			cout << endl;
		}
	}
}



//���ݷ��ս�����ҵ����в���ʽ�����Ҫ����հ�����Ŀ 
set<stateElem> newXiangMu(set<char> ch) {
	set<stateElem> xiangMus;
	for (set<char>::iterator it = ch.begin(); it != ch.end(); ++it) { // �������ս�� 
		for (int i = 0; i < generationNum; i++) { // ��������ʽ 
			if (generations[i].left == *it) { // ��������ŵĲ���ʽ 
				stateElem now = {false,0,generations[i], i + 1};
				xiangMus.insert(now);
				// ������������ս���ܷ�������µ���Ҫ������Ŀ���ķ��ս�� 
				if (nonTerminate.find(generations[i].right[0]) != nonTerminate.end() && ch.find(generations[i].right[0]) == ch.end()) { 
					ch.insert(generations[i].right[0]);
					it = ch.begin();
				}
			}
		}
	}
	return xiangMus;
}

//�հ�������A -> a��Xb����closure(I),��XΪ���ս����,��X -> ��...����closure(I)
void closure(state& s) {
	
	set<char> targetCh;//�ռ���ǰ��Ŀ������Բ��֮��ķ��ս����
	for (set<stateElem>::iterator it = s.elem.begin(); it != s.elem.end(); ++it) {
		if (it->finish)//��Լ��Ŀ������
			continue;
		char ch = it->gen.right[it->pos];
		if (Terminate.find(ch) != Terminate.end())//Բ��֮��ķ���Ϊ�ս����,��Ҫ 
			continue;
		targetCh.insert(ch);// �������������ս�� 
	}
	set<stateElem> xiangMus = newXiangMu(targetCh);
	for (set<stateElem>::iterator it = xiangMus.begin(); it != xiangMus.end(); ++it) //��������Ŀ
		s.elem.insert(*it);
	
	
	s.hasReduce = false;
	bool judgeShift = true;
	int reduceNum = 0;// ��Լ��ͻ������ 
	for (set<stateElem>::iterator it = s.elem.begin(); it != s.elem.end(); ++it) {
		judgeShift &= it->finish;
		if (it->finish) {
			s.hasReduce = true;//�й�Լ��Ŀ
			reduceNum++;
			continue;
		}
		char ch = it->gen.right[it->pos];
		s.chs.insert(ch);
	}
	if (judgeShift)//�޺����Ŀ�����ж�Ҫyijin���������Ŀ�����ƽ��� 
		s.canShift = false;
	else s.canShift = true;//�к����Ŀ
	s.conflict1 = s.canShift & s.hasReduce ;//�ж��ƽ���Լ��ͻ 
	s.conflict2 = reduceNum > 1 ? true : false; //��Լ��ͻ 
}

//ת�Ʋ��� I1 = closure(I0, ch)
state go(const state& s, char ch) {
	//��ת�Ƶĳ�����Ŀ������newState��; 2. ��newState����closure����
	state newState;
	//����s��������Ŀ
	for (set<stateElem>::iterator it = s.elem.begin(); it != s.elem.end(); ++it) {
		if (it->finish)//��ǰ��ĿΪ��Լ��Ŀ������
			continue;
		if (it->gen.right[it->pos] == ch) {//��ǰ��Ŀ��Բ���ķ���Ϊch
			stateElem newStateElem;
			newStateElem = *it;
			newStateElem.pos++;//Բ�����һλ
			if (newStateElem.pos == newStateElem.gen.right.size())
				newStateElem.finish = true;
			newState.elem.insert(newStateElem);//��ת�ƺ����Ŀ��������Ŀ��
		}
	}
	closure(newState);//������Ŀ�����бհ�����
	return newState;
}


void genDFA() {//����DFA
	//���ȹ����̬��
	stateNum++;
	states[0].number = 0;
	stateElem firstElem = {false, 0, generations[0], 1};//��һ����Ŀ��Q-> S
//	cout << firstElem.gen.left<<" -> "<< firstElem.gen.right  << endl;
	states[0].elem.insert(firstElem);
	closure(states[0]);//�հ���������̬��Ŀ���������
	for (int i = 0; i < stateNum; i++) {
		if (states[i].canShift == false)// �����жϵ�ǰ��Ŀ���Ƿ��к����Ŀ
			continue;
		for (set<char>::iterator it = states[i].chs.begin(); it != states[i].chs.end(); ++it) {//�оʹ����ٵķ���chs������ 
			bool canPut = true;//����Ƿ�������Ŀ������
			state newState = go(states[i], *it);//ת�Ʋ���,�����µ���Ŀ��
			//����
			for (int k = 0; k < stateNum; k++) {
				if (newState == states[k]) {//��ʱ��ζ��states[i] --> states[k](ͨ��*it)
					Query q = { i,*it };
					stateDFA[q] = k;//����DFA��״̬iͨ��*itת����״̬k
					canPut = false;
					break;
				}
			}
			if (!canPut)//������Ŀ��������������һ��ת�Ʋ���
				continue;
			newState.number = stateNum;
			states[stateNum++] = newState;
			Query q = { i,*it };
			stateDFA[q] = newState.number;//����DFA��״̬iͨ��*itת����״̬stateNum-1(����״̬)
		}
	}
}


void showStateElem(const stateElem& s) {
	if (s.finish) {
		cout << s.gen.left << " -> " << s.gen.right;
		cout << "��" << endl;
	}
	else {
		int len = s.gen.right.size();
		cout << s.gen.left << " -> ";
		for (int i = 0; i < len; i++) {
			if (i == s.pos)
				cout << "��";
			cout << s.gen.right[i];
		}
		cout << endl;
	}
}

void showStates() {
	for (int i = 0; i < stateNum; i++) {
		cout << i << "����Ŀ��:" << endl;
		for (set<stateElem>::iterator it = states[i].elem.begin(); it != states[i].elem.end(); ++it) {
			showStateElem(*it);
		}
	}
}

//�ж����������Ƿ��н���
bool hasCommonElem(const set<char>& t1, const set<char>& t2) {
	for (set<char>::const_iterator it = t1.begin(); it != t1.end(); ++it) {//ÿ��ȡ��t1��һ��Ԫ����t2���ң����ҵ����н���
		if (t2.find(*it) != t2.end())//�ҵ���
			return true;
	}
	return false;//������t1������Ԫ��Ҳû����t2���ҵ���˵���޽���
}

//�жϳ�ͻ�ܷ���
bool canSolved(const vector<set<char> >& t) {
	int len = t.size();
	for (int i = 0; i < len; ++i)
		for (int j = i + 1; j < len; ++j)
			if (hasCommonElem(t[i], t[j]))//���������н���
				return false;
	return true;
}

//����SLR���������������򷵻�true�����򷵻�false
bool genSLRTable() {//����SLR������,����true��ʾ���ķ�ΪSLR(1)�ķ�
	for (int i = 0; i < stateNum; i++) {//������Ŀ��
		bool conflict = states[i].conflict1 || states[i].conflict2;
		if (conflict) {//�����жϳ�ͻ�Ƿ�ɽ��
			vector<set<char> > temp;//���ڴ�ų�ͻ��Ŀ��follow����Բ��֮���Ԫ�ؼ���
			set<char> shiftSet;//Բ��֮���Ԫ�ؼ���
			for (set<stateElem>::iterator it = states[i].elem.begin(); it != states[i].elem.end(); ++it) {//������Ŀ����������Ŀ
				if (it->finish)//��Լ��Ŀ
					temp.push_back(follow[it->gen.left]);//������ʽ�󲿵�follow������
				else shiftSet.insert(it->gen.right[it->pos]);//�ƽ���Ŀ����Բ���ķ��ż�������
			}
			temp.push_back(shiftSet);//��ʱtemp�д�����й�Լ��Ŀ�󲿵�follow�����ƽ���Ŀ��Բ��֮��ķ���
			if (!canSolved(temp)) {//���ܽ����ͻ������false
				cout << "��ͻ��Ŀ��" << i << "���ܽ����ͻ!" << endl;
				return false;
			}
		}
		//���Խ����ͻ
		for (set<stateElem>::iterator it = states[i].elem.begin(); it != states[i].elem.end(); ++it) {//������Ŀ����������Ŀ
			if (it->finish) {//��Լ��Ŀ
				if (it->gen.left == BEGIN && it->pos == it->gen.right.size()) {//������Ŀ
					Query q = { i,'#' };
					slr s = { 'a',-1 };
					table[q] = s;
				}
				else {//A -> ...����������follow(A)�з���a, ��action[i,a] = rj
					for (set<char>::iterator vit = follow[it->gen.left].begin(); vit != follow[it->gen.left].end(); ++vit) {
						Query q = { i,*vit };//��ǰ״̬Ϊi,���ٷ���Ϊ*vit
						slr s = { 'r',it->No};//��it->No+1�Ų���ʽ��Լ
						table[q] = s;
					}
				}
			}
			else {//�ƽ���Ŀ
				char ch = it->gen.right[it->pos];//Բ���ķ���
				Query q = { i,ch };//��ǰ״̬Ϊi,���ٷ���Ϊch
				slr s = { 's',stateDFA[q] };//stateDFA[q]Ϊת�Ƶ�״̬��
				table[q] = s;
			}
		}
	}
	return true;
}

void showGenerations() {	//����ʽ������ 
	for (int i = 0; i < generationNum; i++)
		cout << i + 1 << "�Ų���ʽ��" << generations[i].left << " -> " << generations[i].right << endl;
}

void showSLRTable() {//��ʾSLR������'
	
	cout << "SLR(1)������������ʾ:" << endl;
	cout << "\t\t\tACTION\t\t\t\t\t\t\t\tGOTO" << endl;
	cout << "\t";
	for (set<char>::iterator it = Terminate.begin(); it != Terminate.end(); ++it)
		cout << *it << "\t";
	cout << '#' << "\t";
	for (set<char>::iterator it = nonTerminate.begin(); it != nonTerminate.end(); ++it)
		cout << *it << "\t";
	
	cout << endl;
	for (int i = 0; i < stateNum; i++) {
		cout << i << "\t";
		for (set<char>::iterator it = Terminate.begin(); it != Terminate.end(); ++it) {//�ս����
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
		for (set<char>::iterator it = nonTerminate.begin(); it != nonTerminate.end(); ++it) {//���ս����
			Query q = {i, *it};
			if (table.find(q) != table.end()) {
				cout << table[q].num;
			}
			cout << "\t";
		}
		cout << endl;
	}
}

//��ӡ״̬ջ
void printStateStack() {
	int len = state_stack.size();
	for (int i = 0; i < len; i++)
		cout << state_stack[i] << " ";
}

//��ӡ�ַ�ջ
void printChStack() {
	int len = ch_stack.size();
	for (int i = 0; i < len; i++)
		cout << ch_stack[i];
}

char newTemp(){
	char c = 'A';
	result_count++;
	return c + result_count - 1;
}


//�������
void semantic(char reduceCh, string juBing) {
	int len = juBing.size();
	if (len == 1) {
		if (Terminate.find(juBing[0]) != Terminate.end()) {//������ս����
			symbol_table[reduceCh].push_back(juBing[0]);
		}
		else {//���Ϊ���ս����
			symbol_table[reduceCh].push_back(symbol_table[juBing[0]].back());
			symbol_table[juBing[0]].pop_back();
		}
	}
	else {
		if (juBing[0] == '(') {// F ��(E)
			symbol_table[reduceCh].push_back(symbol_table[juBing[1]].back());
			symbol_table[juBing[1]].pop_back();
		}
		else {//���������Ҫ������Ԫʽ
			if (juBing[1] == '=') {//A ��V=E
				siYuanShi siyuanshi = { '=',symbol_table[juBing[2]].back(),'_',symbol_table[juBing[0]].back() };
				symbol_table[juBing[2]].pop_back();
				symbol_table[juBing[0]].pop_back();
				siYuanShis.push_back(siyuanshi);
			}
			else {// +��-��*��/
				char arg1 = symbol_table[juBing[0]].back();
				symbol_table[juBing[0]].pop_back();
				char arg2 = symbol_table[juBing[2]].back();
				symbol_table[juBing[2]].pop_back();
				symbol_table[reduceCh].push_back(newTemp());//��ʱ����
				siYuanShi siyuanshi = { juBing[1],arg1,arg2,symbol_table[reduceCh].back() };
				siYuanShis.push_back(siyuanshi);
			}
		}
	}
}

bool SLR(FILE* fp) {//SLR����
	char inputCh;//�������
	
	state_stack.push_back(0);
	ch_stack.push_back('#');
	current = getCurrent(fp);
	int procedure = 1;
	cout << "����\t״̬ջ\t����ջ\t�������\t��������\t��һ״̬\n";
	while (!feof(fp)) {
		if (current.id == -1)
			break;
		if (current.id == ID) {
			inputCh = current.word[0];
			if (inputCh != 'i') {
				cout << "����δ�����ʶ��!" << endl;
				return false;
			}

		}
		else inputCh = convertion[current.id][0];
		cout << procedure++ << "\t";
		Query q = { state_stack.back(),inputCh };
		if (table.find(q) == table.end()) {
			cout << "SLR(1)����ʧ��" << endl;
			return false;
		}
		if (table[q].kind == 's') {//ת��
			printStateStack();
			cout << "\t";
			printChStack();
			cout << "\t";
			cout << inputCh << "\t";
			cout << table[q].kind << table[q].num << "\t" << table[q].num << endl;
			
			ch_stack.push_back(inputCh);//�����������ջ
			state_stack.push_back(table[q].num);//��ת��״̬��ջ
			current = getCurrent(fp);//���봮����һλ
		}
		else if (table[q].kind == 'r') {//��Լ
			printStateStack();
			cout << "\t";
			printChStack();
			cout << "\t";
			cout << inputCh << "\t";
			cout << table[q].kind << table[q].num << "\t";
			char reduceCh = generations[table[q].num - 1].left;//��Լ����
			int reduceNum = generations[table[q].num - 1].right.size();//����ĳ���
			semantic(reduceCh,generations[table[q].num - 1].right);//�������嶯��
			//��������ͬ��������ջ��״̬ջ�����ݳ�ջ(����Ϊ����ĳ���), Ȼ����״̬ջ����״̬�͹�Լ���Ų�ѯSLR����������״̬��ջ
			while (reduceNum) {
				state_stack.pop_back();
				ch_stack.pop_back();
				reduceNum--;
			}
			ch_stack.push_back(reduceCh);//����Լ������ջ
			Query q = { state_stack.back(),reduceCh };
			cout << "GOTO[" << state_stack.back() << ',' << reduceCh << "]=" << table[q].num <<endl;
			
			state_stack.push_back(table[q].num);//����״̬��ջ
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

//��ʾ��Ԫʽ
void showSiYuanShi() {
	int len = siYuanShis.size();
	for (int i = 0; i < len; i++)
		cout << '(' << siYuanShis[i].op << ',' << siYuanShis[i].arg1 << ',' << siYuanShis[i].arg2 << ',' << siYuanShis[i].res << ')' << endl;
}

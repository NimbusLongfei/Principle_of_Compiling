#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <string>
using namespace std;

int token_index;

typedef string Token;

// 产生式规则结构
struct ProductionRule {
    vector<string> production;
};

// 文法规则
map<string, vector<ProductionRule>> grammar;

// First集和Follow集
map<string, set<string>> first;
map<string, set<string>> follow;



void readInput(vector<Token>& input_tokens) {
    string input_str;
    cout << "Enter the input string (without spaces): ";
    cin >> input_str;

    // 将输入字符串拆分为单个终结符，并存储在 input_tokens 中
    for (char symbol : input_str) {
        input_tokens.push_back(string(1, symbol));
    }
    input_tokens.push_back("#");
}


// 添加产生式到文法规则
void addProductionRule(string non_terminal, vector<string> production) {
    ProductionRule rule;
    rule.production = production;
    grammar[non_terminal].push_back(rule);
}

// 计算First集
set<string> calculateFirst(string symbol) {
    if (symbol[0] >= 'A' && symbol[0] <= 'Z') {
        set<string> result;
        for (const ProductionRule& rule : grammar[symbol]) {
            int i = 0;
            while (i < rule.production.size()) {
                set<string> first_of_symbol = calculateFirst(rule.production[i]);
                result.insert(first_of_symbol.begin(), first_of_symbol.end());
                if (first_of_symbol.find("ε") == first_of_symbol.end()) {
                    break;
                }
                i++;
            }
        }
        return result;
    }
    else {
        return { symbol };
    }
}
// 计算所有非终结符的First集
void calculateAllFirstSets() {
    for (const auto& rule : grammar) {
        if (first.find(rule.first) == first.end()) {
            first[rule.first] = calculateFirst(rule.first);
        }
    }
}

void calculateFollow() {
    // 初始化所有非终结符的Follow集合为空
    for (const auto& rule : grammar) {
        if (follow.find(rule.first) == follow.end()) {
            follow[rule.first] = set<string>();
        }
    }

    // 将'#'（结束标识符）添加到开始符号（S）的Follow集合中
    follow["S"].insert("#");

    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& rule : grammar) {
            for (const ProductionRule& production_rule : rule.second) {
                for (int i = 0; i < production_rule.production.size(); i++) {
                    const string& symbol = production_rule.production[i];

                    if (symbol[0] >= 'A' && symbol[0] <= 'Z') {
                        // 处理当前符号
                        set<string> follow_of_rule;

                        int j = i + 1;
                        bool can_derive_empty = true;

                        while (j < production_rule.production.size() && can_derive_empty) {
                            // 处理当前符号之后的符号
                            set<string> first_of_next = calculateFirst(production_rule.production[j]);
                            follow_of_rule.insert(first_of_next.begin(), first_of_next.end());

                            if (first_of_next.find("ε") == first_of_next.end()) {
                                // 如果没有ε在First集合中，停止迭代
                                can_derive_empty = false;
                            }
                            j++;
                        }

                        if (i == production_rule.production.size() - 1 || can_derive_empty) {
                            // 如果Xi是产生式的最后一个符号或者包含ε，将非终结符的Follow集合合并
                            follow_of_rule.insert(follow[rule.first].begin(), follow[rule.first].end());
                        }

                        // 将计算的Follow集合合并到对应的符号的Follow集合中
                        size_t initial_size = follow[symbol].size();
                        follow[symbol].insert(follow_of_rule.begin(), follow_of_rule.end());

                        // 如果Follow集合发生变化，继续迭代
                        if (follow[symbol].size() > initial_size) {
                            changed = true;
                        }
                    }
                }
            }
        }
    }
    // 移除包含空串的Follow集合中的空串
    for (auto& entry : follow) {
        entry.second.erase("ε");
    }
}




// 初始化文法
void initializeGrammar() {
    addProductionRule("S", { "V", "=", "E" });
    addProductionRule("E", { "T", "E'" });
    addProductionRule("E'", { "A", "T", "E'" });
    addProductionRule("E'", { "ε" });
    addProductionRule("T", { "F", "T'" });
    addProductionRule("T'", { "M", "F", "T'" });
    addProductionRule("T'", { "ε" });
    addProductionRule("F", { "(", "E", ")" });
    addProductionRule("F", { "i" });
    addProductionRule("A", { "+" });
    addProductionRule("A", { "-" });
    addProductionRule("M", { "*" });
    addProductionRule("M", { "/" });
    addProductionRule("V", { "i" });
}

// 预测分析表
map<string, map<string, vector<string>>> parsing_table;

// 构建预测分析表
void buildParsingTable() {
    // 初始化预测分析表
    parsing_table.clear();

    // 遍历文法规则
    for (const auto& rule : grammar) {
        const string& non_terminal = rule.first;
        const vector<ProductionRule>& production_rules = rule.second;

        for (const ProductionRule& production_rule : production_rules) {
            // 计算First集合
            set<string> first_set = calculateFirst(production_rule.production[0]);

            // 遍历First集合中的终结符
            for (const string& terminal : first_set) {
                if (terminal != "ε") {
                    // 将产生式右侧符号串包装在一个vector中，并关联到非终结符和终结符的对应表格项
                    vector<string> production;
                    production.insert(production.end(), production_rule.production.begin(), production_rule.production.end());
                    parsing_table[non_terminal][terminal] = production;
                }
            }

            // 如果First集合包含ε，将Follow集合中的终结符也加入预测分析表
            if (first_set.count("ε") > 0) {
                const set<string>& follow_set = follow[non_terminal];

                for (const string& terminal : follow_set) {
                    // 只有当Follow集合中的终结符不在First集合中时，才将其添加到预测分析表
                    if (parsing_table[non_terminal].find(terminal) == parsing_table[non_terminal].end()) {
                        // 包装产生式右侧符号串在一个vector中
                        vector<string> production;
                        production.insert(production.end(), production_rule.production.begin(), production_rule.production.end());
                        parsing_table[non_terminal][terminal] = production;
                    }
                }
            }
        }
    }

    // 输出预测分析表
    cout << "Parsing Table:" << endl;
    for (const auto& non_terminal_entry : parsing_table) {
        const string& non_terminal = non_terminal_entry.first;

        for (const auto& terminal_entry : non_terminal_entry.second) {
            const string& terminal = terminal_entry.first;
            const vector<string>& production = terminal_entry.second;

            cout << "M[" << non_terminal << ", " << terminal << "] = ";
            for (const string& symbol : production) {
                cout << symbol << " ";
            }
            cout << endl;
        }
    }
}




// 分析栈
vector<string> stack;

// 将符号推入分析栈
void pushToStack(const string& symbol) {
    stack.push_back(symbol);
}

// 从分析栈中弹出符号
void popFromStack() {
    stack.pop_back();
}

// 打印分析栈内容
void printStack() {
    cout << "Stack: ";
    for (const string& symbol : stack) {
        cout << symbol << " ";
    }
    cout << endl;
}

// 解析输入串
void parseInput(const vector<Token>& input_tokens) {
    pushToStack("#");
    pushToStack("S");
    int token_index = 0;

    while (!stack.empty() && token_index < input_tokens.size()) {
        printStack();
        if (stack.back() == input_tokens[token_index] ) {
            popFromStack();
            token_index++;
        }
        else if (stack.back() == "ε") {
            popFromStack();
        }
        else if (stack.back()[0] >= 'A' && stack.back()[0] <= 'Z') {
            string non_terminal = stack.back();
            if (parsing_table.find(non_terminal) != parsing_table.end() &&
                parsing_table[non_terminal].find(input_tokens[token_index]) != parsing_table[non_terminal].end()) {
//                printStack();
                popFromStack();
                vector<string> production = parsing_table[non_terminal][input_tokens[token_index]];
                for (int i = production.size() - 1; i >= 0; i--) {
                    pushToStack(production[i]);
                }
            }
            else {
                cout << "Error: Parsing table entry not found" << endl;
                exit(1);
            }
        }
        else {
            cout << "Error: Mismatch between stack and input token" << endl;
            exit(1);
        }
    }

    // 检查是否成功分析整个输入串
    if (stack.empty() && token_index == input_tokens.size()) {
        cout << "Input successfully parsed!" << endl;
    }
    else {
        cout << "Error: Unable to parse the input" << endl;
        return;
    }
}




int main() {
    // 从用户输入获取词法分析的二元组
    vector<Token> input_tokens;
    readInput(input_tokens);

    // 初始化全局变量
    token_index = 0;

    // 初始化文法
    initializeGrammar();

    // 计算First集和Follow集
    calculateAllFirstSets();
// 输出 First 集合
    cout << "First Sets:" << endl;
    for (const auto& entry : first) {
        const string& non_terminal = entry.first;
        const set<string>& first_set = entry.second;

        cout << non_terminal << ": { ";
        for (const string& symbol : first_set) {
            cout << symbol << " ";
        }
        cout << "}" << endl;
    }


    calculateFollow();




//// 输出 Follow 集合
    cout << "Follow Sets:" << endl;
    for (const auto& entry : follow) {
        const string& non_terminal = entry.first;
        const set<string>& follow_set = entry.second;

        cout << non_terminal << ": { ";
        for (const string& symbol : follow_set) {
            cout << symbol << " ";
        }
        cout << "}" << endl;
    }

    // 构建预测分析表
    buildParsingTable();
//    // 输出预测分析表
//    cout << "Parsing Table:" << endl;
//
//    for (const auto& non_terminal_entry : parsing_table) {
//        const string& non_terminal = non_terminal_entry.first;
//
//        // 遍历终结符
//        for (const auto& terminal_entry : non_terminal_entry.second) {
//            const string& terminal = terminal_entry.first;
//            const string& production = terminal_entry.second;
//
//            cout << "M[" << non_terminal << ", " << terminal << "] = " << production << endl;
//        }
//    }

    // 解析输入串
    parseInput(input_tokens);


    return 0;
}
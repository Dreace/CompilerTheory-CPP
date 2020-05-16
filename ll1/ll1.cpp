#include <iostream>
#include <stack>
#include "ll1.h"

using std::cout;
using std::stack;

Symbol::Symbol(string symbol, SYMBOL_TYPE type) : symbol(std::move(symbol)), type(type) {}

bool Symbol::operator==(const Symbol &right) const {
    return symbol == right.symbol;
}

bool Symbol::operator<(const Symbol &right) const {
    return symbol < right.symbol;
}

Production::Production(const string &str) {
    left = Symbol(string(1, str[0]), VARIABLE);
    this->add_candidate(str);
}

void Production::add_candidate(const string &str) {
    if (str.length() < 4 || !(str[1] == '-' && str[2] == '>')) {
        is_valid = false;
        return;
    }
    int length = str.length();
    int start = 3, end;
    for (end = start + 1; end <= length; ++end) {
        if (str[end] == '|' || end == length) {
            vector<Symbol> candidate;
            for (int i = start; i < end; ++i) {
                SYMBOL_TYPE type;
                if (str[i] == '$') {
                    type = EMPTY;
                } else if (str[i] >= 'A' && str[i] <= 'Z') {
                    type = VARIABLE;
                } else {
                    type = TERMINAL;
                }
                candidate.emplace_back(string(1, str[i]), type);
            }
            candidates.emplace_back(candidate);
            start = end + 1;
        }
    }
}

LL1::LL1(string &production_input) {
    int start = 0, end;
    int length = production_input.length();
    map<char, int> temp_map;
    int count = 0;
    for (end = start + 1; end <= length; ++end) {
        if (end - start > 1 && (production_input[end] == '\n' || end == length)) {
            string str = production_input.substr(start, end - start);
            if (temp_map.find(str[0]) != temp_map.end()) {  // 左部已存在
                productions[temp_map[str[0]]].add_candidate(str);
            } else {
                temp_map[str[0]] = count++;
                productions.emplace_back(Production(str));
            }
            start = end + 1;
        }
    }
    for (const auto &production:productions) {
        for (const auto &candidate:production.candidates) {
            for (const auto &symbol:candidate) {
                first[symbol] = set<Symbol>();
            }
        }
        follow[production.left] = set<Symbol>();
    }
    done.clear();
    for (const auto &production:productions) {
        calculate_first(production.left);
    }
    for (const auto &production:productions) {
        for (const auto &candidate:production.candidates) {
            string str;
            for (auto const &symbol:candidate) {
                str += symbol.symbol;
            }
            string_first[str] = set<Symbol>();
            for (auto symbol_it = candidate.begin(); symbol_it != candidate.end(); ++symbol_it) {
                auto temp(first[*symbol_it]);
                if (symbol_it + 1 != candidate.end()) {
                    temp.erase(Symbol());
                }
                string_first[str].insert(temp.begin(), temp.end());
                if (first[*symbol_it].find(Symbol()) == first[*symbol_it].end()) {    // 当前符号不能推出 $
                    break;
                }
            }
        }
    }
    done.clear();
    for (const auto &production:productions) {
        calculate_follow(production.left);
    }
    // 以下为信息输出
    for (const auto &x:first) {
        cout << x.first.symbol << ":";
        for (const auto &y:x.second) {
            cout << y.symbol << " ";
        }
        cout << "\n";
    }
    cout<<"----------\n";
    for (const auto &x:string_first) {
        cout << x.first << ":";
        for (const auto &y:x.second) {
            cout << y.symbol << " ";
        }
        cout << "\n";
    }
    cout<<"----------\n";
    for (const auto &x:follow) {
        cout << x.first.symbol << ":";
        for (const auto &y:x.second) {
            cout << y.symbol << " ";
        }
        cout << "\n";
    }
    construct_analyse_table();
    cout<<"----------\n";
    for (const auto &x:analyse_table) {
        cout << x.first.symbol << ": ";
        for (const auto &y:x.second) {
            cout << "(" << y.first.symbol << ",";
            string str;
            for (auto const &symbol:y.second) {
                str += symbol.symbol;
            }
            cout << str << ") ";
        }
        cout << "\n";
    }
}

void LL1::calculate_first(const Symbol &symbol) {
    if (done.find(symbol) != done.end()) {
        return;
    }
    if (symbol.type == TERMINAL || symbol.type == EMPTY) {
        first[symbol].insert(symbol);
        done[symbol] = true;
        return;
    }
    for (const auto &production:productions) {
        if (production.left == symbol) {
            for (const auto &candidate: production.candidates) {
                for (auto symbol_it = candidate.begin(); symbol_it != candidate.end(); ++symbol_it) {
                    calculate_first(*symbol_it);
                    if (symbol_it->type != VARIABLE || first[*symbol_it].find(Symbol()) == first[*symbol_it].end()) {
                        first[symbol].insert(first[*symbol_it].begin(), first[*symbol_it].end());
                        break;
                    }
                    auto temp(first[*symbol_it]);
                    if (symbol_it + 1 != candidate.end()) {
                        temp.erase(Symbol());
                    }
                    first[symbol].insert(temp.begin(), temp.end());
                }
            }
            break;
        }
    }
    done[symbol] = true;
}

void LL1::calculate_follow(const Symbol &symbol) {
    if (done.find(symbol) != done.end()) {
        return;
    } else {
        done[symbol] = true;
    }
    if (symbol.symbol == "S") {
        follow[symbol].insert(Symbol("#", TERMINAL));
        done[symbol] = true;
    }
    for (const auto &production:productions) {
        for (const auto &candidate:production.candidates) {
            for (auto symbol_it = candidate.begin(); symbol_it != candidate.end(); ++symbol_it) {
                if ((*symbol_it) == symbol) {
                    if (symbol_it + 1 == candidate.end()) { // 最后一个符号
                        calculate_follow(production.left);
                        follow[*symbol_it].insert(follow[production.left].begin(), follow[production.left].end());
                        continue;
                    }
                    if ((symbol_it + 1)->type == TERMINAL) {
                        follow[*symbol_it].insert(*(symbol_it + 1));
                    } else {
                        auto temp_set(first[*(symbol_it + 1)]);
                        temp_set.erase(Symbol());
                        follow[*symbol_it].insert(temp_set.begin(), temp_set.end());
                        if (first[*(symbol_it + 1)].find(Symbol()) != first[*(symbol_it + 1)].end()) {
                            calculate_follow(production.left);
                            follow[*(symbol_it)].insert(follow[production.left].begin(),
                                                        follow[production.left].end());
                        }
                    }
                }
            }
        }
    }
}

void LL1::construct_analyse_table() {
    for (const auto &production:productions) {
        analyse_table[production.left] = map<Symbol, vector<Symbol>>();
        for (const auto &candidate:production.candidates) {
            string str;
            for (auto const &symbol:candidate) {
                str += symbol.symbol;
            }
            for (const auto &symbol:string_first[str]) {
                if (symbol.type == EMPTY) {
                    continue;
                }
                analyse_table[production.left][symbol] = candidate;
            }
            if (string_first[str].find(Symbol()) != string_first[str].end()) {
                for (const auto &symbol:follow[production.left]) {
                    if (symbol.type == EMPTY) {
                        continue;
                    }
                    analyse_table[production.left][symbol] = candidate;
                }
                if (follow[production.left].find(Symbol("#", TERMINAL)) != follow[production.left].end()) {
                    analyse_table[production.left][Symbol("#", TERMINAL)] = candidate;
                }
            }
        }
    }
}


vector<vector<string>> LL1::analyse(string &input) {
    vector<vector<string>> res;
    vector<Symbol> analyse_stack;
    vector<Symbol> symbols;
    for (auto s:input) {
        symbols.emplace_back(Symbol(string(1, s), TERMINAL));
    }
    analyse_stack.emplace_back(Symbol("#", TERMINAL));
    analyse_stack.emplace_back(Symbol("S", VARIABLE));
    auto it = symbols.begin();
    while (it != symbols.end()) {
        if (analyse_stack.size() == 1) {
            vector<string> temp(3);
            temp[0] = temp[1] = "#";
            temp[2] = "分析成功";
            res.emplace_back(temp);
            break;
        }
        if (analyse_stack.back().type == TERMINAL && analyse_stack.back() == *it) {   // 终结符匹配
            vector<string> temp(3);
            temp[0] = symbol_vector_to_string(analyse_stack);
            temp[1] = symbol_vector_to_string(vector<Symbol>(it, symbols.end()));
            temp[2] = it->symbol + " 匹配";
            res.emplace_back(temp);
            analyse_stack.pop_back();
            ++it;
            continue;
        }
        if (analyse_table[analyse_stack.back()].find(*it) != analyse_table[analyse_stack.back()].end()) {
            vector<string> temp(3);
            temp[0] = symbol_vector_to_string(analyse_stack);
            temp[1] = symbol_vector_to_string(vector<Symbol>(it, symbols.end()));
            temp[2] = analyse_stack.back().symbol + "->" +
                      symbol_vector_to_string(analyse_table[analyse_stack.back()][*it]);
            Symbol back = analyse_stack.back();
            analyse_stack.pop_back();
            auto symbol_it = analyse_table[back][*it].end();
            if ((symbol_it - 1)->type != EMPTY) {
                while (true) {
                    --symbol_it;
                    analyse_stack.emplace_back(*symbol_it);
                    if (symbol_it == analyse_table[back][*it].begin()) {
                        break;
                    }
                }
            }
            res.emplace_back(temp);
        }
    }
    return res;
}

string LL1::symbol_vector_to_string(const vector<Symbol> &v) {
    string str;
    for (const auto &s:v) {
        str += s.symbol;
    }
    return str;
}

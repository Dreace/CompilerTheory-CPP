#ifndef COMPILING_PRINCIPLE_LL1_H
#define COMPILING_PRINCIPLE_LL1_H

#include <string>
#include <vector>
#include <set>
#include <utility>
#include <map>

using std::string;
using std::set;
using std::vector;
using std::pair;
using std::map;
enum SYMBOL_TYPE {
    EMPTY,
    VARIABLE,
    TERMINAL,
};

class Symbol {
    friend class Production;

    friend class LL1;

private:
public:
    explicit Symbol(string symbol = "$", SYMBOL_TYPE type = EMPTY);

    bool operator==(const Symbol &right) const;

    bool operator<(const Symbol &right) const;

private:
    string symbol;
    SYMBOL_TYPE type;
};

class Production {
    friend class LL1;

public:
    explicit Production(const string &str);

private:
    bool is_valid = true; // 是否合法
    Symbol left = Symbol();  // 左部
    vector<vector<Symbol>> candidates;   // 候选式
    void add_candidate(const string &str);
};

class LL1 {
public:
    explicit LL1(string &production_input);

    vector<vector<string>> analyse(string &input);

private:
    vector<Production> productions;
    map<Symbol, set<Symbol>> first;
    map<string, set<Symbol>> string_first;
    map<Symbol, set<Symbol>> follow;
    map<Symbol, map<Symbol, vector<Symbol>>> analyse_table;
    map<Symbol, bool> done;

    void calculate_first(const Symbol &symbol);

    void calculate_follow(const Symbol &symbol);

    void construct_analyse_table();
    static string symbol_vector_to_string(const vector<Symbol> &v);
};


#endif //COMPILING_PRINCIPLE_LL1_H

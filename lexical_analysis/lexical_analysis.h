#ifndef COMPILING_PRINCIPLE_LEXICAL_ANALYSIS_H
#define COMPILING_PRINCIPLE_LEXICAL_ANALYSIS_H

#include <map>
#include <vector>
#include <utility>
#include <string>

using std::map;
using std::vector;
using std::pair;
using std::string;

enum CLangType {
    ERROR,                  // 错误
    KEY_WORD,               // 保留字
    DELIMITER,              // 定界符
    OPERATOR,               // 算数符
    NUMBER,                 // 数字
    IDENTIFIER,             // 标识符
    CHAR,                   // 字符
    STRING,                 // 字符串
    COMMENT,                // 注释
    MULTIPLE_LINE_COMMENT,  // 多行注释
};

class Result {
public:
    explicit Result(string token = "", CLangType type = ERROR, int line = -1, int column = -1);

public:
    CLangType type;
    int line{};
    int column = -1;
    string token;
};

class Lexical {
private:
    map<string, CLangType> recognized;

    static map<string, int> key_word_id_map;
    static map<string, CLangType> delimiters_and_operators;      // 运算符
    static const char *type_strings[];
    static const char *key_words[];


    static bool isNumber(const string &str);

    static bool isKeyWord(const string &str);

    static bool isIdentifier(const string &str);

    static bool isOperator(const string &str);

    static bool isOperator(char c);

public:
    Lexical();

    vector<Result> analysis(string input);

    static string cLangTypeToString(CLangType cLangType);
};

#endif //COMPILING_PRINCIPLE_LEXICAL_ANALYSIS_H

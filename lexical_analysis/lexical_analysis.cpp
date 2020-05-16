#include <iostream>
#include <utility>
#include "lexical_analysis.h"

map<string, int> Lexical::key_word_id_map{};
map<string, CLangType> Lexical::delimiters_and_operators{};
const char *Lexical::type_strings[] = {
        "ERROR", "KEY_WORD", "DELIMITER", "OPERATOR", "NUMBER", "IDENTIFIER",
        "CHAR", "STRING", "COMMENT", "MULTIPLE_LINE_COMMENT",
};
const char *Lexical::key_words[] = {
        "auto", "double", "int", "struct", "break", "else", "long",
        "switch", "case", "enum", "register", "typedef", "char",
        "extern", "return", "union", "const", "float", "short",
        "unsigned", "continue", "for", "signed", "void", "default",
        "goto", "sizeof", "volatile", "do", "if", "while", "static",
};

Lexical::Lexical() {
    int id = 0;
    for (const auto &str:key_words) {
        key_word_id_map.insert({str, id});
    }
    const char *delimiters[] = {",", ";", "(", ")", "[", "]", "{", "}",};
    for (const auto &delimiter:delimiters) {
        delimiters_and_operators.insert({delimiter, DELIMITER});
    }
    const char *operators[] = {"+", "-", "*", "/", "%", "++", "--", "<<", ">>", "&", "|", "^",
                               "~", ">", "<", ">=", "<=", "!=", "==", "=", "||", "&&", "!", "+=",
                               "-=", "*=", "/=", "&=", "|=", "^=", "<<=", ">>=", "->", ".", "sizeof",
                               "?", ":"};
    for (const auto &operator_:operators) {
        delimiters_and_operators.insert({operator_, OPERATOR});
    }
}

vector<Result> Lexical::analysis(string input) {
    input.append(" ");
    vector<Result> results;
    int input_length = input.length();
    int lines = 1;
    int column = 0, column_start_at = 0;
    while (column < input_length) {
        Result result;
        result.column = column;
        result.line = lines;
        char c = input[column];
        int j;
        string str;
        if (c == '\n') {
            ++lines;
            ++column;
            column_start_at = column;
            continue;
        } else if (isalpha(c)) {    // 标识符
            for (j = column + 1; j < input_length && (isalnum(input[j]) || input[j] == '_'); ++j);
            str = input.substr(column, j - column);
            if (isKeyWord(str)) {
                result.type = KEY_WORD;
            } else if (isIdentifier(str)) {
                result.type = IDENTIFIER;
            } else {
                result.type = ERROR;
            }
            column = j;
        } else if (isdigit(c)) {    //数字
            for (j = column + 1; j < input_length && isalnum(input[j]); ++j);
            str = input.substr(column, j - column);
            result.type = isNumber(str) ? NUMBER : ERROR;
            column = j;
        } else if (c == '/' && (input[column + 1] == '*' || input[column + 1] == '/')) {
            if (input[column + 1] == '*') {   //多行注释
                char *c_str = const_cast<char *>(input.c_str());
                for (j = column + 2; j < input_length - 2; ++j) {
                    if (c_str[j] == '*' && c_str[j + 1] == '/') {
                        str = input.substr(column + 2, j - column - 2);
                        column = j + 2;
                        break;
                    }
                }
                if (j < input_length - 2) {
                    result.type = MULTIPLE_LINE_COMMENT;
                } else {
                    str = input.substr(column, input_length - column);
                    result.type = ERROR;
                    column = input_length;
                }

            } else {    //单行注释
                for (j = column + 2; j < input_length && input[j] != '\n'; ++j);
                str = input.substr(column + 2, j - column - 2);
                result.type = COMMENT;
                column = j;
            }
        } else if (isOperator(c)) { //各种运算符与定界符
            if (delimiters_and_operators[string(1, c)] == DELIMITER) {
                result.type = DELIMITER;
                str = string(1, c);
                ++column;
            } else {
                for (j = column + 1; j < input_length && isOperator(input.substr(column, j - column)); ++j);
                str = input.substr(column, j - column - 1);
                result.type = delimiters_and_operators[str];
                column = j - 1;
            }
        } else if (c == '"' || c == '\'') {
            for (j = column + 1; j < input_length; ++j) {
                if (j == '\\') {    // 转义符起始，跳过下一个字符
                    ++j;
                } else if (input[j] == c) {
                    break;
                }
            }
            str = input.substr(column + 1, j - column - 1);
            if (j >= input_length) {
                result.type = ERROR;
            } else if (c == '"') {
                result.type = STRING;
            } else {
                if (str.length() == 1 || (str[0] == '\\' && str.length() == 2)) {
                    result.type = CHAR;
                } else {
                    string sub = str.substr(1, str.length() - 1);
                    if (isNumber(sub) && strtol(sub.c_str(), nullptr, 10) < 377) {
                        result.type = CHAR;
                    } else {
                        result.type = ERROR;
                    }
                }
            }
            column = j + 1;
        } else {
            ++column;
            continue;
        }
        result.token = str;
        result.column -= (column_start_at - 1);
        this->recognized.insert({str, result.type});
        results.emplace_back(result);
        Result result1;
        Result *p_result = &result1;
        Result **p_p_result = &p_result;
        Result ***pResult = &p_p_result;
    }
    return results;
}

string Lexical::cLangTypeToString(CLangType cLangType) {
    return type_strings[cLangType];
}

bool Lexical::isNumber(const string &str) {
    for (auto c:str) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

bool Lexical::isKeyWord(const string &str) {
    return key_word_id_map.find(str) != key_word_id_map.end();
}

bool Lexical::isIdentifier(const string &str) {
    if (!isalnum(str[0]) && str[0] != '_') {
        return false;
    }
    for (const auto &c:str) {
        if (!isalnum(c) && str[0] != '_') {
            return false;
        }
    }
    return true;
}

bool Lexical::isOperator(const string &str) {
    return delimiters_and_operators.find(str) != delimiters_and_operators.end();
}

bool Lexical::isOperator(char c) {
    return isOperator(string(1, c));
}

Result::Result(string token, CLangType type, int line, int column) : token(std::move(token)), type(type), line(line),
                                                                     column(column) {

}

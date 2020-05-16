#include <iostream>
#include <sstream>

#include "lexical_analysis.h"

using std::cout;
using std::cin;
using std::getline;

string print_space(uint32_t n) {
    string str;
    while (n--) {
        str += " ";
    }
    return str;
}

int main() {
    string input;
    getline(cin, input, '~');
    Lexical lexical;
    for (const auto &result:lexical.analysis(input)) {
        cout << result.token << " => " << Lexical::cLangTypeToString(result.type);
        if (result.type == ERROR) {
            cout << "(" << result.line << "," << result.column << ")";
        }
        cout << "\n";
    }
    return 0;
}

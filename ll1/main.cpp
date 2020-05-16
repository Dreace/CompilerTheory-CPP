#include <iostream>
#include <sstream>

#include "ll1.h"

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
    LL1 ll1(input);
    getchar();
    getline(cin, input, '~');
    int cnt = 0;
    cout<<"----------\n";
    for (auto line:ll1.analyse(input)) {
        cout << cnt << "\t" << line[0] << print_space(20 - line[0].length() - line[1].length()) << line[1]
             << "\t" << line[2] << "\n";
        ++cnt;
    }
    return 0;
}

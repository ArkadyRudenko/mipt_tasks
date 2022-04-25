#include "String.h"

std::ostream& operator<<(std::ostream& out, const String& str) {
    for (int i = 0; i < str.size(); i++) {
        out << str[i];
    }
    return out;
}

void operator>>(std::istream &in, String &str) {
    while (!in.eof()) {
        char ch;
        in >> ch;
        str.push_back(ch);
    }
}
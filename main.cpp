#include <iostream>

#include "./src/Lexer.cc"

using namespace  std;

int main(int argc, char **argv) {
    while (true) {
        int strtok = gettok();
        cout << strtok << "\tNum: " << NumVal << "\tStr: " << IdentifierString  << endl;
    }
}


/*!
 * @author Imanol
 * @date 3/6/22
 * @project hebe
 */

#include "Lexer.h"


/**
 * This gettok() function recognizes the text and returns the token value of the parsed input
 * @return token value
 */
int gettok() {
    static int LastChar = ' ';

    // ignore space chars
    while (isspace(LastChar))
        LastChar = getchar();

    // recognize identifiers and keywords
    if (isalpha(LastChar)) {    // [a-zA-Z][a-zA-Z0-9]*
        IdentifierStr = LastChar;
        while (isalnum((LastChar = getchar())))
            IdentifierStr += LastChar;
        if (IdentifierStr == "def")
            return tok_def;
        if (IdentifierStr == "extern")
            return tok_extern;
        return tok_identifier;
    }

    // recognize numbers
    if (isdigit(LastChar) || LastChar == '.') { // [0-9.]+
        std::string NumString;
        do {
            NumString += LastChar;
            LastChar = getchar();
        } while (isdigit(LastChar) || LastChar == '.');

        NumVal = strtod(NumString.c_str(), nullptr);
        return tok_number;
    }

    // recognize comments
    if (LastChar == '#') {
        do {
            LastChar = getchar();
        } while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

        if (LastChar != EOF)
            return gettok();
    }

    if (LastChar == EOF)
        return tok_eof;

    int ThisChar = LastChar;
    LastChar = getchar();
    return ThisChar;
}
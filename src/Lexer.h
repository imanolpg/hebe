#ifndef LEXER_H
#define LEXER_H

#include <string>

/**
 * Token values
 */
enum Token {
    // EOF token
    tok_eof = -1,

    // comands
    tok_def = -2,
    tok_extern = -3,

    // primary
    tok_identifier = -4,
    tok_number = -5
};


static std::string IdentifierStr;  // value if token is tok_identifier
static double NumVal;           // value if token is tok_number


/**
 * This gettok() function recognizes the text and returns the token value of the parsed input
 * @return token value
 */
int gettok();

#endif
/*!
 * @author Imanol
 * @date 3/6/22
 * @project hebe
 */

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
static int gettok() {
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

        NumVal = strtod(NumString.c_str(), 0);
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
}
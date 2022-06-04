/*!
 * @author Imanol
 * @date 3/6/22
 * @project hebe
 */

#include <map>

#include "Lexer.cc"
#include "AbstractSyntaxTree.cc"

static std::unique_ptr<ExprAST> ParseExpression ();
static std::unique_ptr<ExprAST> ParseBinOpRHS (int ExprPrec, std::unique_ptr<ExprAST> LHS);

static int CurTok;
static int getNextToken() {
    return CurTok = gettok();
}

std::unique_ptr<ExprAST> LogError(const char *Str) {
    fprintf(stderr, "LogError: %s\n", Str);
    return nullptr;
}

std::unique_ptr<PrototypeAST> LogErrorP(const char *Str) {
    LogError(Str);
    return nullptr;
}

/**
 * Parse numeric literals
 * @return ExprAST numeric value
 */
static std::unique_ptr<ExprAST> ParseNumberExpr() {
    auto Result = std::make_unique<NumberExprAST>(NumVal);
    getNextToken(); // consume the number
    return std::move(Result);
}

/**
 * Manage open and close parenthesis
 * @return ExprAST parsed
 */
static std::unique_ptr<ExprAST> ParseParenExpr() {
    getNextToken(); // eat (.
    auto V = ParseExpression();
    if (!V)
        return nullptr;

    if (CurTok != ')')
        return LogError("expected ')'");
    getNextToken(); // eat ).
    return V;
}


/**
 * Parse identifiers expression
 * @return
 */
static std::unique_ptr<ExprAST> ParseIdentifierExpr() {
    std::string IdNames = IdentifierStr;

    getNextToken();

    if (CurTok != '(')
        return std::make_unique<VariableExprAST>(IdNames);

    getNextToken();
    std::vector<std::unique_ptr<ExprAST>> Args;
    if (CurTok != ')') {
        while (true) {
            if (auto Arg = ParseExpression())
                Args.push_back(std::move(Arg));
            else
                return nullptr;

            if(CurTok == ')')
                break;

            if (CurTok != ',')
                return LogError("Expected ')' or ',' in argument list");

            getNextToken();
        }
    }
}


/**
 * Primary function to determine the parsing type is needed
 * @return ExprAST object from parsing logic
 */
static std::unique_ptr<ExprAST> ParsePrimary() {
    switch (CurTok) {
        case tok_identifier:
            return ParseIdentifierExpr();
        case tok_number:
            return ParseNumberExpr();
        case '(':
            return ParseParenExpr();
        default:
            return LogError("unknown token when expecting an expression");
    }
}


// Binary Expressions

static std::map<char, int> BinopPrecedence; // precendence for the binary operations

/**
 * Primary binoprhs
 * @return
 */
static std::unique_ptr<ExprAST> ParseExpression () {
    auto LHS = ParsePrimary();
    if (!LHS)
        return nullptr;

    return ParseBinOpRHS(0, std::move(LHS));
}


/**
 * Gets the preference of the binary operator
 * @return preference of the binary operator
 */
static int GetTokPrecedence() {
    if (!isascii(CurTok))
        return -1;

    // make sure the operator is declared
    int TokPrec = BinopPrecedence[CurTok];
    if (TokPrec <= 0)
        return -1;

    return TokPrec;
}


/**
 * Check if the current token precedence is too low
 * @param ExprPrec precedence to compare with
 * @param LHS left hand side operand
 * @return LHS if the predecende is lower than the LHS of the argument
 */
static std::unique_ptr<ExprAST> ParseBinOpRHS (int ExprPrec,
                                               std::unique_ptr<ExprAST> LHS) {
    while (true) {
        int TokPrec = GetTokPrecedence();

        if (TokPrec < ExprPrec)
            return LHS;

        int BinOp = CurTok;
        getNextToken();

        auto RHS = ParsePrimary();
        if (!RHS)
            return nullptr;

        int NextPrec = GetTokPrecedence();
        if (TokPrec < NextPrec) {
            RHS = ParseBinOpRHS(TokPrec + 1, std::move(RHS));
            if (!RHS)
                return nullptr;
        }

        LHS = std::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
    }
}


// Parsing the rest

/**
 * Parse prototypes
 * @return PrototypeAST object
 */
static std::unique_ptr<PrototypeAST> ParsePrototype() {
    if (CurTok != tok_identifier)
        return LogErrorP("Expected function name in prototype");

    std::string FnName = IdentifierStr;
    getNextToken();

    if (CurTok != '(')
        return LogErrorP("Expected '(' in prototype");

    std::vector<std::string> ArgNames;
    while (getNextToken() == tok_identifier)
        ArgNames.push_back(IdentifierStr);
    if (CurTok != ')')
        return LogErrorP("Expected ')' in prototype");

    getNextToken();

    return std::make_unique<PrototypeAST>(FnName, std::move(ArgNames));
}


/**
 * 'def' prototype expression
 * @return
 */
static std::unique_ptr<FunctionAST> HandleDefinition() {
    getNextToken();  // eat def.
    auto Proto = ParsePrototype();
    if (!Proto) return nullptr;

    if (auto E = ParseExpression())
        return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
    return nullptr;
}


/**
 * 'extern' prototype
 * @return nested ParsePrototype function
 */
static std::unique_ptr<PrototypeAST> HandleExtern() {
    getNextToken();
    return ParsePrototype();
}


/**
 * Top level expressions
 * @return null
 */
static std::unique_ptr<FunctionAST> HandleTopLevelExpression() {
    if (auto E = ParseExpression()) {
        // Make an anonymous proto.
        auto Proto = std::make_unique<PrototypeAST>("", std::vector<std::string>());
        return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));
    }
    return nullptr;
}


// main loop

static void MainLoop() {
    // Install standard binary operators.
    // 1 is lowest precedence.
    BinopPrecedence['<'] = 10;
    BinopPrecedence['+'] = 20;
    BinopPrecedence['-'] = 20;
    BinopPrecedence['*'] = 40;

    // Prime the first token.
    fprintf(stderr, "ready> ");
    getNextToken();

    while (true) {
        fprintf(stderr, "ready> ");
        switch (CurTok) {
            case tok_eof:
                return;
            case ';': // ignore top-level semicolons.
                getNextToken();
                break;
            case tok_def:
                HandleDefinition();
                break;
            case tok_extern:
                HandleExtern();
                break;
            default:
                HandleTopLevelExpression();
                break;
        }
    }
}
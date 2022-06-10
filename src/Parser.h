#ifndef PARSER_H
#define PARSER_H

#include <map>

#include "AbstractSyntaxTree.h"
#include "Lexer.h"


static int CurTok;
static int getNextToken();


/**
 * Error logging function. Prints the error message on stderr output
 * @param Str message to print
 * @return null pointer
 */
std::unique_ptr<ExprAST> LogError(const char *Str);


/**
 * Error logging function for prototypes. Handles the return type. It calls LogError function to log the error
 * @param Str message to log
 * @return null pointer
 */
std::unique_ptr<PrototypeAST> LogErrorP(const char *Str);


/**
 * Arranges the preference of expressions
 * @return BinOps
 */
std::unique_ptr<ExprAST> ParseExpression();


/**
 * Parse numeric literals
 * @return ExprAST numeric value
 */
std::unique_ptr<ExprAST> ParseNumberExpr();


/**
 * Manage open and close parenthesis
 * @return ExprAST parsed
 */
std::unique_ptr<ExprAST> ParseParenExpr();


/**
 * Parse identifiers expression
 * @return
 */
std::unique_ptr<ExprAST> ParseIdentifierExpr();


/**
 * Primary function to determine the parsing type is needed
 * @return ExprAST object from parsing logic
 */
std::unique_ptr<ExprAST> ParsePrimary();


/**
 * Check if the current token precedence is too low
 * @param ExprPrec precedence to compare with
 * @param LHS left hand side operand
 * @return LHS if the predecende is lower than the LHS of the argument
 */
std::unique_ptr<ExprAST> ParseBinOpRHS (int ExprPrec, std::unique_ptr<ExprAST> LHS);


//===----------------------------------------------------------------------===//
// Binary Expressions
//===----------------------------------------------------------------------===//

static std::map<char, int> BinopPrecedence; // precendence for the binary operations

/**
 * Primary binoprhs
 * @return
 */
std::unique_ptr<ExprAST> ParseExpression();


/**
 * Gets the preference of the binary operator
 * @return preference of the binary operator
 */
int GetTokPrecedence();


/**
 * Check if the current token precedence is too low
 * @param ExprPrec precedence to compare with
 * @param LHS left hand side operand
 * @return LHS if the predecende is lower than the LHS of the argument
 */
std::unique_ptr<ExprAST> ParseBinOpRHS (int ExprPrec, std::unique_ptr<ExprAST> LHS);


//===----------------------------------------------------------------------===//
// Parsing the rest
//===----------------------------------------------------------------------===//

/**
 * Parse prototypes
 * @return PrototypeAST object
 */
std::unique_ptr<PrototypeAST> ParsePrototype();


/**
 * 'def' prototype expression
 * @return
 */
std::unique_ptr<FunctionAST> HandleDefinition();


/**
 * 'extern' prototype
 * @return nested ParsePrototype function
 */
std::unique_ptr<PrototypeAST> HandleExtern();


/**
 * Top level expressions
 * @return null
 */
std::unique_ptr<FunctionAST> HandleTopLevelExpression();


/**
 * Main loop
 */
void MainLoop();

#endif
#ifndef PARSER_H
#define PARSER_H

#include <map>

#include "AbstractSyntaxTree.h"
#include "Lexer.h"


std::unique_ptr<ExprAST> ParseExpression ();
std::unique_ptr<ExprAST> ParseBinOpRHS (int ExprPrec, std::unique_ptr<ExprAST> LHS);

static int CurTok;
int getNextToken();

std::unique_ptr<ExprAST> LogError(const char *Str);

std::unique_ptr<PrototypeAST> LogErrorP(const char *Str);

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


// Binary Expressions

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


// Parsing the rest

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


// main loop
void MainLoop();

#endif
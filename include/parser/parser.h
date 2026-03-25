#pragma once

#include <memory>
#include <stdexcept>

#include "ast/ast.h"

extern int yyparse(); // Declaration of the parsing function.
extern ASTNode* root; // Defined in grammar.
extern FILE* yyin;    // Defined in grammar.

namespace parser {

/**
 * @brief Handler to close the file automatically.
 *
 * @details
 * When passing FileCloser as the second template in @ref UniqueFile "UniqueFile", UniqueFile needs
 * to be a type. This is due to the fact that _Templates_ are resolved at compile time.
 *
 * @note The call operator `operator()` makes the struct callable. Example of `operator()` usage:
 * @code{.cpp}
 *  struct FileCloser {
 *   void operator()(FILE* f) { fclose(f); }
 *  };
 *
 *  FileCloser closer;
 *  closer(f)
 * @endcode
 *
 */
struct FileCloser {
  void operator()(FILE* f) const {
    if (f)
      fclose(f);
  }
};

/**
 * @brief Alias for automatically file closing.
 *
 * @details
 * The files of this type can be left without manually calling `fclose()`, as they will be
 * automatically closed when the `UniqueFile` goes out of scope. It is used with @ref readFile
 * "readFile" function.
 */
using UniqueFile = std::unique_ptr<FILE, FileCloser>;

/**
 * @brief Reads a file.
 *
 * @details
 * This function is used to read the content of `.hebe` files. It takes the file name as input,
 * attempts to open the file, and returns a unique pointer to the opened file. If the file cannot be
 * opened, it throws an exception with an appropriate error message.
 *
 * @param fileName name of the Hebe file to load the code from.
 * @return FILE* pointer to the open file.
 * @throws std::ios_base::failure if the file cannot be opened.
 */
UniqueFile readFile(char* fileName);

/**
 * @brief Parses the input code and return the root node of the AST.
 *
 * @param source file pointer to the source code to be parsed.
 * @return ASTNode* pointer to the root node of the parsed AST.
 * @throws parser::SyntaxError if a syntax error is encountered.
 * @throws parser::MemoryExhaustion if memory is exhausted during parsing.
 */
ASTNode* parseCode(FILE* source);

// =================================================================================================
// Errors

/**
 * @brief Exception thrown the code of a file contains syntactic errors.
 *
 * @details
 * An example of this can be a file containing incorrect code. Any syntactic or lexical error in the
 * provided code will throw this exception.
 */
class SyntaxError : public std::runtime_error {
public:
  explicit SyntaxError(const std::string& message) : std::runtime_error(message) {}
};

/**
 * @brief Exception thrown when parsing fails.
 *
 * @details
 * This exception is thrown when the parser uses more memory than is available, which can occur when
 * the input code is too large.
 */
class MemoryExhaustion : public std::runtime_error {
public:
  explicit MemoryExhaustion(const std::string& message) : std::runtime_error(message) {}
};

}; // namespace parser
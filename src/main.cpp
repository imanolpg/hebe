#include "main.h"

#include <cstdlib>
#include <ios>

#include "ast/ast.h"
#include "codegen/codegen.h"
#include "logging.h"
#include "parser/parser.h"

int main(int argc, char** argv) {

  parser::UniqueFile sourceCode;
  ASTNode* rootNode;

  int exitCode;

  // Allow only one param.
  if (argc != 2) {
    logsys::get()->error("Only one argument is expected.");
    return -1;
  }

  // Read the code from the file.
  try {
    sourceCode = parser::readFile(argv[1]);
  } catch (const std::ios_base::failure& e) {
    logsys::get()->error("Error reading code file: {}", e.what());
    return -1;
  }

  // Parse the input code.
  try {
    rootNode = parser::parseCode(sourceCode.get());
  } catch (const parser::SyntaxError& e) {
    logsys::get()->error("Error parsing code: {}", e.what());
    return -1;
  } catch (const parser::MemoryExhaustion& e) {
    logsys::get()->error("Error parsing code: {}", e.what());
    return -1;
  } catch (const std::exception& e) {
    logsys::get()->error("Unexpected error during parsing: {}", e.what());
    return -1;
  }

  // Call yyparse() and check its return value for errors
  CodeGen compiler = CodeGen(rootNode);
  compiler.generateCode();
  if (isDebug)
    compiler.printNodeTree();
  if (isDebug)
    compiler.printLLVMIR();
  if (isDebug)
    compiler.exportIRToFile("output_code.ll");
  exitCode = compiler.runJIT();

  return exitCode;
}

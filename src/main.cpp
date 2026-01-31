#include <string>

#include "ast/ast.h"
#include "compiler.h"
#include "logging.h"

extern int yyparse(); // Declaration of the parsing function.
extern ASTNode* root; // Defined in grammar.
extern FILE* yyin;    // Defined in grammar.

int main(int argc, char** argv) {

  // Read code file.
  if (argc > 1) {
    yyin = fopen(argv[1], "r");
    if (!yyin) {
      perror("fopen");
      return 1;
    }
  }

  int exitCode;

  // Call yyparse() and check its return value for errors
  if (yyparse() == 0) {
    Compiler compiler = Compiler(root);
    compiler.generateCode();
    compiler.printNodeTree();
    compiler.printLLVMIR();
    compiler.exportIRToFile("output_code.ll");
    exitCode = compiler.runJIT();
  } else {
    logsys::get()->error("Parsing error occurred!");
    return 1;
  }

  return exitCode;
}

/*!
 * @author Imanol
 * @date 31/7/22
 * @project hebe
 */

#include "JITDriver.h"
#include "CodeGeneration.h"
#include "Parser.h"


void InitializeModule() {
    // create context and module
    TheContext = std::make_unique<llvm::LLVMContext>();
    TheModule = std::make_unique<llvm::Module>("my cool jit", *TheContext);

    // create a builder for the module
    Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
}


void HandleDefinition() {
    if (auto FnAST = ParseDefinition()) {
        if (auto *FnIR = FnAST->codegen()) {
            fprintf(stderr, "Read function definition:");
            FnIR->print(llvm::errs());
            fprintf(stderr, "\n");
        }
    } else {
        // Skip token for error recovery.
        getNextToken();
    }
}


void HandleExtern() {
    if (auto ProtoAST = ParseExtern()) {
        if (auto *FnIR = ProtoAST->codegen()) {
            fprintf(stderr, "Read extern: ");
            FnIR->print(llvm::errs());
            fprintf(stderr, "\n");
        }
    } else {
        // Skip token for error recovery.
        getNextToken();
    }
}


void HandleTopLevelExpression() {
    // evaluate the top level expression into an anonymousfunction
    if (auto FnAST = ParseTopLevelExpr()) {
        if (auto *FnIR = FnAST->codegen()) {
            fprintf(stderr, "Read top-level expression:");
            FnIR->print(llvm::errs());
            fprintf(stderr, "\n");
            // Remove the anonymous expression.
            FnIR->eraseFromParent();
        }
    } else {
        // Skip token for error recovery.
        getNextToken();
    }
}


#ifndef CODEGENERATION_H
#define CODEGENERATION_H


#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"

#include "AbstractSyntaxTree.h"
#include "Parser.h"


static std::unique_ptr<llvm::LLVMContext> TheContext;
static std::unique_ptr<llvm::IRBuilder<>> Builder;
static std::unique_ptr<llvm::Module> TheModule;
static std::map<std::string, llvm::Value *> NamedValues;

/**
 * Error loggin function
 * @param Str message
 * @return null pointer
 */
llvm::Value *LogErrorV(const char *Str);


llvm::Value *NumberExprAST();

llvm::Value *VariableExprAST();

llvm::Value *BinaryExprAST();

llvm::Value *CallExprAST();

llvm::Function *PrototypeAST();

llvm::Function *FunctionAST();

#endif
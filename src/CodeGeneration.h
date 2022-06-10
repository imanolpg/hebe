#ifndef CODEGENERATION_H
#define CODEGENERATION_H


#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"

#include "AbstractSyntaxTree.h"
#include "Parser.h"


llvm::LLVMContext TheContext;
llvm::IRBuilder<> Builder(TheContext);
std::unique_ptr<llvm::Module> TheModule;
std::map<std::string, llvm::Value *> NamedValues;

llvm::Value *LogErrorV(const char *Str);


#endif
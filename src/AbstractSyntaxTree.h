#ifndef ABSTRACTSYNTAXTREE_H
#define ABSTRACTSYNTAXTREE_H


#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

#include <memory>
#include <string>
#include <vector>


/**
 * Base class for all nodes
 */
class ExprAST {
public:
    virtual ~ExprAST();
    virtual llvm::Value *codegen();
};


/**
 * Number node class
 */
class NumberExprAST : public ExprAST {
    double Val;

public:
    NumberExprAST(double Val_V);
    llvm::Value *codegen() override;
};


/**
 * Variable ndoe class
 */
class VariableExprAST : public ExprAST {
    std::string Name;

public:
    VariableExprAST(const std::string &Name);
    llvm::Value *codegen() override;
};


/**
 * Operator node class
 */
class BinaryExprAST : public ExprAST {
    char Op;
    std::unique_ptr<ExprAST> LHS, RHS;

public:
    BinaryExprAST(char op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS);
    llvm::Value *codegen() override;
};


/**
 * Function calls node class
 */
class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;

public:
    CallExprAST(const std::string &Callee, std::vector<std::unique_ptr<ExprAST>> Args);
    llvm::Value *codegen() override;
};


/**
 * Function Prototype node class
 */
class PrototypeAST {
    std::string Name;
    std::vector<std::string> Args;

public:
    PrototypeAST(const std::string &Name, std::vector<std::string> Args);
    llvm::Function *codegen();
    const std::string &getName();
};


/**
 * Function definition node class
 */
class FunctionAST {
    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<ExprAST> Body;

public:
    FunctionAST(std::unique_ptr<PrototypeAST> Proto, std::unique_ptr<ExprAST> Body);
    llvm::Function *codegen();
};


#endif
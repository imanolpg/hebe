/*!
 * @author Imanol
 * @date 3/6/22
 * @project hebe
 */

#include "AbstractSyntaxTree.h"


/**
 * ExprAST class
 */
ExprAST::~ExprAST() = default;
llvm::Value *ExprAST::codegen() {}


/**
 * NumberExprAST
 */
 NumberExprAST::NumberExprAST(double Val_V) : Val(Val_V) {}


/**
 * VariableExprAST
 */
VariableExprAST::VariableExprAST(const std::string &Name) : Name(Name) {}


/**
 * BinaryExprAST
 */
 BinaryExprAST::BinaryExprAST(char op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS) :
         Op(op), LHS(std::move(LHS)), RHS(std::move(RHS))  {}


/**
 * CallExprAST
 */
CallExprAST::CallExprAST(const std::string &Callee, std::vector<std::unique_ptr<ExprAST>> Args) :
        Callee(Callee), Args(std::move(Args)) {}


/**
 * PrototypeAST class
 */
PrototypeAST::PrototypeAST(const std::string &Name, std::vector<std::string> Args) : Name(Name),
        Args(std::move(Args)) {}
const std::string &PrototypeAST::getName() { return Name; }


/**
 * Function definition node class
 */
FunctionAST::FunctionAST(std::unique_ptr<PrototypeAST> Proto, std::unique_ptr<ExprAST> Body) :
        Proto(std::move(Proto)), Body(std::move(Body)) {}
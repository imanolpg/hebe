#include "codegen/codegen.h"

#include "ast/programNode.h"
#include "logging.h"

llvm::Value* CodeGen::codegenExpr(ASTNode* node) {
  if (!node)
    return nullptr;
  switch (node->type) {
  case NodeType::Number:
    return this->codegenNumber(node);
  case NodeType::BinaryOp:
    return this->codegenBinaryOp(node);
  case NodeType::Assignment:
    return this->codegenAssignment(node);
  case NodeType::ProcedureBody:
    return this->codegenProcedureBody(node);
  case NodeType::Procedure:
    return this->codegenProcedure(node);
  case NodeType::ProcedureCall:
    return this->codegenProcedureCall(node);
  default:
    logsys::get()->error("Code generation for type {} not supported.", getNodeType(node->type));
    throw std::runtime_error("Code generation for this type of node not supported");
  }
}

void CodeGen::generateCode() {
  logsys::get()->info("Executing generateCode");

  // Ensure there is a list of nodes.
  if (!this->rootNode) {
    logsys::get()->error("No code provided. rootNode is empty");
    throw std::runtime_error("Failed to generate code.");
  }

  // Create main function where the code will run.
  llvm::FunctionType* mainFuncTy = this->createFunctionType(llvm::Type::getFloatTy(*this->context));
  this->getOrCreateFunction("run", mainFuncTy);

  // Create the basic block that will be executed on program start.
  llvm::BasicBlock* entry = this->createBasicBlock("entry", "run");
  this->builder->SetInsertPoint(entry);

  // Create variable last alloca to avoid lose of instructions after optimization passes.
  // FIXME: this has to be removed. I only put this for debugging because the IR optimization would
  // remove all expressions that had a result without being stored.
  llvm::AllocaInst* lastAlloca =
      this->builder->CreateAlloca(llvm::Type::getFloatTy(*this->context), nullptr, "forDebug");

  llvm::Value* expr = nullptr;

  ProgramNode* program = static_cast<ProgramNode*>(this->rootNode);
  for (ASTNode* node : program->getItems()) {
    expr = this->codegenExpr(node);

    // FIXME: remove this, only for debugging. Storing instructions already stores desired
    // variables.
    if (node->type == NodeType::Number || node->type == NodeType::BinaryOp)
      this->builder->CreateStore(expr, lastAlloca);
  }

  if (expr) {
    llvm::Constant* retPtr = this->getOrCreateGlobalVariable("ret");
    llvm::LoadInst* retValue =
        this->builder->CreateLoad(llvm::Type::getFloatTy(*this->context), retPtr);
    this->builder->CreateRet(retValue);
  } else {
    logsys::get()->error("Failed to generate code. Last evaluated expression has an error.");
    throw std::runtime_error("Failed to generate code. Last evaluated expression has an error.");
  }
}

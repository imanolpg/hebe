#include "ast/procedure/procedureBodyNode.h"
#include "ast/procedure/procedureCallNode.h"
#include "ast/procedure/procedureNode.h"
#include "codegen/codegen.h"

#include "logging.h"

llvm::Value* CodeGen::codegenProcedureBody(ASTNode* inputNode) {
  ProcedureBodyNode* node = static_cast<ProcedureBodyNode*>(inputNode);

  // Create variable last alloca to avoid lose of instructions after optimization passes.
  // FIXME: this has to be removed. I only put this for debugging because the IR optimization would
  // remove all expressions that had a result without being stored.
  llvm::AllocaInst* lastAlloca =
      this->builder->CreateAlloca(llvm::Type::getFloatTy(*this->context), nullptr, "forDebug");

  llvm::Value* expr = nullptr;

  for (ASTNode* child : node->getItems()) {
    expr = this->codegenExpr(child);

    // FIXME: remove this, only for debugging. Storing instructions already stores desired
    // variables.
    if (child->type == NodeType::Number || child->type == NodeType::BinaryOp)
      this->builder->CreateStore(expr, lastAlloca);
  }

  return nullptr;
}

llvm::Value* CodeGen::codegenProcedure(ASTNode* inputNode) {

  ProcedureNode* node = static_cast<ProcedureNode*>(inputNode);

  // Get the function type.
  llvm::FunctionType* fnTy = this->createFunctionType(llvm::Type::getVoidTy(*this->context));

  // Create the function.
  llvm::Function* fnPtr = this->createFunction(node->name, fnTy);

  // Get a new basic block for the new function.
  llvm::BasicBlock* bbPtr = this->createBasicBlock("entry", node->name);

  // Save the basic block where the builder was inserting instructions to restore it later.
  llvm::BasicBlock* oldBbPtr = this->builder->GetInsertBlock();

  // Set function insert point.
  this->builder->SetInsertPoint(bbPtr);

  // Parse the child instructions.
  this->codegenProcedureBody(node->body);

  // Return void as the function is void type.
  this->builder->CreateRetVoid();

  // Restore the old basic block to continue inserting instructions in the parent function.
  this->builder->SetInsertPoint(oldBbPtr);

  return fnPtr;
}

llvm::Value* CodeGen::codegenProcedureCall(ASTNode* inputNode) {
  ProcedureCallNode* node = static_cast<ProcedureCallNode*>(inputNode);

  // Get the procedure pointer to create a call instruction.
  llvm::Function* procPtr = this->module->getFunction(node->name);

  if (!procPtr) {
    logsys::get()->error("Function {} not found in llvm module", node->name);
    throw std::runtime_error("Function not found in llvm module");
  }

  // Call the procedure.
  llvm::Value* retVal = this->builder->CreateCall(procPtr);

  return retVal;
}

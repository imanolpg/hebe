#include "ast/assignmentNode.h"
#include "codegen/codegen.h"

llvm::Value* CodeGen::codegenAssignment(ASTNode* inputNode) {
  AssignmentNode* node = static_cast<AssignmentNode*>(inputNode);

  // Get the global variable pointer.
  llvm::Constant* variablePtr = this->getOrCreateGlobalVariable(node->name);

  // Parse the possible expression of the variable value.
  llvm::Value* variableValue = this->codegenExpr(node->value);

  // Store the value in the variable.
  builder->CreateStore(variableValue, variablePtr);

  return variablePtr; // FIXME: a type of Value* should be returned and now is returning
                      // llvm::Constant*
}
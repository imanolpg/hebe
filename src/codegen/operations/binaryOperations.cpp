#include "ast/binaryOpNode.h"
#include "codegen/codegen.h"

#include "logging.h"

llvm::Value* CodeGen::codegenBinaryOp(ASTNode* inputNode) {
  BinaryOpNode* node = static_cast<BinaryOpNode*>(inputNode);

  llvm::Value* leftExpr = this->codegenExpr(node->left);
  llvm::Value* rightExpr = this->codegenExpr(node->right);

  switch (node->op) {
  case '+':
    return builder->CreateFAdd(leftExpr, rightExpr, "addtmp");
  case '-':
    return builder->CreateFSub(leftExpr, rightExpr, "subtmp");
  case '*':
    return builder->CreateFMul(leftExpr, rightExpr, "multmp");
  case '/':
    return builder->CreateFDiv(leftExpr, rightExpr, "divtmp");
  default:
    logsys::get()->error("Operation '{}' not supported", node->op);
    return nullptr;
  }
}

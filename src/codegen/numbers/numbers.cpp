#include "ast/numberNode.h"
#include "codegen/codegen.h"

llvm::Value* CodeGen::codegenNumber(ASTNode* inputNode) {
  NumberNode* node = static_cast<NumberNode*>(inputNode);
  return llvm::ConstantFP::get(llvm::Type::getFloatTy(*this->context), node->value);
}
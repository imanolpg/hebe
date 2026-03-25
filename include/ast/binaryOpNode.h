#pragma once

#include "ast/ast.h"
#include "ast/nodeType.h"

/**
 * @brief Math expression nodes.
 * E.g. 42.0 + 2.0 | BinaryOpNode + 42.0
 *
 */
class BinaryOpNode : public ASTNode {
public:
  char op;
  ASTNode* left;
  ASTNode* right;
  BinaryOpNode(char o, ASTNode* l, ASTNode* r)
      : ASTNode(NodeType::BinaryOp), op(o), left(l), right(r) {
    type = NodeType::BinaryOp;
  }

  ~BinaryOpNode() override {
    delete left;
    delete right;
  }
};

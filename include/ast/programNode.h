#pragma once

#include <vector>

#include "ast.h"

/**
 * @brief General program node.
 * It is the main node containing all the expressions.
 *
 */
class ProgramNode : public ASTNode {
  std::vector<ASTNode*> items;

public:
  ProgramNode() : ASTNode(NodeType::Program) {}

  ~ProgramNode() override {
    for (ASTNode* n : items)
      delete n;
  }

  void append(ASTNode* n) {
    if (n)
      items.push_back(n);
  }
  std::vector<ASTNode*> getItems() { return items; }
};

#pragma once

#include <vector>

#include "ast/ast.h"

/**
 * @brief Procedure body node.
 * Contains an instruction list that go inside a ProcedureNode.
 *
 */
class ProcedureBodyNode : public ASTNode {
  std::vector<ASTNode*> items;

public:
  ProcedureBodyNode() : ASTNode(NodeType::ProcedureBody) {}

  ~ProcedureBodyNode() override {
    for (ASTNode* n : items)
      delete n;
  }

  void append(ASTNode* n) {
    if (n)
      items.push_back(n);
  }

  std::vector<ASTNode*> getItems() { return items; }
};

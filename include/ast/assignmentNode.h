#pragma once

#include "ast/ast.h"

/**
 * @brief Assignment Instruction node.
 * E.g. save 42.0 in x
 *
 */
class AssignmentNode : public ASTNode {
public:
  std::string name;
  ASTNode* value;
  AssignmentNode(std::string name, ASTNode* value)
      : ASTNode(NodeType::Assignment), name(std::move(name)), value(value) {}

  ~AssignmentNode() override { delete value; }
};

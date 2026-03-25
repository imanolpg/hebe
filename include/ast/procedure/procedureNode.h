#pragma once

#include <string>

#include "ast/ast.h"

/**
 * @brief Procedure body node.
 * E.g. CREATE procedure_name PROCEDUREBODY END.
 *
 */
class ProcedureNode : public ASTNode {
public:
  std::string name;
  ASTNode* body;

  ProcedureNode(std::string name, ASTNode* body)
      : ASTNode(NodeType::Procedure), name(std::move(name)), body(body) {}

  ~ProcedureNode() override { delete body; }
};

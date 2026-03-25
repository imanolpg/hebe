#pragma once

#include "ast/ast.h"

/**
 * @brief Calls a procedure.
 * E.g. my_procedure_name.
 *
 */
class ProcedureCallNode : public ASTNode {
public:
  std::string name;

  ProcedureCallNode(std::string name) : ASTNode(NodeType::ProcedureCall), name(std::move(name)) {}
};
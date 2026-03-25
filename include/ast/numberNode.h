#pragma once

#include "ast.h"

/**
 * @brief Number node.
 * E.g. 42.0
 *
 */
class NumberNode : public ASTNode {
public:
  double value;
  NumberNode(double val) : ASTNode(NodeType::Number), value(val) {}
};

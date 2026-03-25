#pragma once

#include <string>

#include "ast/nodeType.h"

// TODO: create an ASTArena to handle the ownership of all AST nodes.

std::string getNodeType(NodeType type);

class ASTNode {
public:
  NodeType type;
  explicit ASTNode(NodeType t) : type(t) {}
  virtual ~ASTNode() = default;
};

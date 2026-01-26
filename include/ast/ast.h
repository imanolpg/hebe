#pragma once

#include <string>
#include <vector>

enum class NodeType {
  Program,
  Number,
  BinaryOp,
  Assignment,
  Procedure,
  ProcedureBody,
  ProcedureCall
};

std::string getNodeType(NodeType type);

class ASTNode {
public:
  NodeType type;
  explicit ASTNode(NodeType t) : type(t) {}
  virtual ~ASTNode() = default;
};

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
};

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
};

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
};

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

#include "ast/ast.h"

std::string getNodeType(NodeType type) {
  switch (type) {
  case NodeType::Program:
    return "Program";
  case NodeType::Number:
    return "Number";
  case NodeType::BinaryOp:
    return "BinaryOp";
  case NodeType::Assignment:
    return "Assignment";
  case NodeType::ProcedureBody:
    return "ProcedureBody";
  case NodeType::Procedure:
    return "Procedure";
  case NodeType::ProcedureCall:
    return "ProcedureCall";
  }
  return "Unknown";
}
#include "ast/assignmentNode.h"
#include "ast/binaryOpNode.h"
#include "ast/numberNode.h"
#include "ast/procedure/procedureBodyNode.h"
#include "ast/procedure/procedureCallNode.h"
#include "ast/procedure/procedureNode.h"
#include "ast/programNode.h"
#include "codegen/codegen.h"
#include "logging.h"

void CodeGen::printNodeTree(ASTNode* node, int depth) {
  // First initialize the node to start printing. This is required because the method can be called
  // without parameters and needs to be initialized.
  if (!node)
    node = this->rootNode;

  // Check that the node has initialized correctly.
  if (!node) {
    logsys::get()->error("Node not initalized. Can not get root node from object.");
    throw std::runtime_error("Node not initialized. Can not get root node from object.");
  }

  switch (node->type) {
  case NodeType::Program: {
    ProgramNode* p = static_cast<ProgramNode*>(node);
    logsys::get()->info("{}Program:", std::string(depth, 't'));
    for (ASTNode* child : p->getItems()) {
      this->printNodeTree(child, depth + 1);
    }
    break;
  }
  case NodeType::Number: {
    logsys::get()->info("{}Number: {}", std::string(depth, '\t'),
                        static_cast<NumberNode*>(node)->value);
    break;
  }
  case NodeType::BinaryOp: {
    BinaryOpNode* binNode = static_cast<BinaryOpNode*>(node);
    logsys::get()->info("{}BinaryOp: {}", std::string(depth, '\t'), binNode->op);
    this->printNodeTree(binNode->left, depth + 1);
    this->printNodeTree(binNode->right, depth + 1);
    break;
  }
  case NodeType::Assignment: {
    AssignmentNode* assNNode = static_cast<AssignmentNode*>(node);
    logsys::get()->info("{}Assignment:", std::string(depth, '\t'));
    logsys::get()->info("{}VariableName: {}", std::string(depth + 1, '\t'), assNNode->name);
    this->printNodeTree(assNNode->value, depth + 1);
    break;
  }
  case NodeType::ProcedureBody: {
    ProcedureBodyNode* procBdyNode = static_cast<ProcedureBodyNode*>(node);
    logsys::get()->info("{}ProcedureBody:", std::string(depth, '\t'));
    for (ASTNode* child : procBdyNode->getItems()) {
      this->printNodeTree(child, depth + 1);
    }
    break;
  }
  case NodeType::Procedure: {
    ProcedureNode* procNode = static_cast<ProcedureNode*>(node);
    logsys::get()->info("{}ProcedureNode:", std::string(depth, '\t'));
    logsys::get()->info("{}Name: {}", std::string(depth + 1, '\t'), procNode->name);
    this->printNodeTree(procNode->body, depth + 1);
    break;
  }
  case NodeType::ProcedureCall: {
    ProcedureCallNode* procCallNode = static_cast<ProcedureCallNode*>(node);
    logsys::get()->info("{}ProcedureCallNode:", std::string(depth, '\t'));
    logsys::get()->info("{}Name: {}", std::string(depth + 1, '\t'), procCallNode->name);
    break;
  }
  default: {
    logsys::get()->error("Printing type {} not supported", getNodeType(node->type));
    break;
  }
  }
  return;
}

void CodeGen::printLLVMIR() {
  // Check if the module has been created.
  if (!this->module) {
    logsys::get()->error("LLVM Module is not initialized");
    throw std::runtime_error("LLVM Module is not initialized");
  }

  // Check if the module has content.
  if (module->empty())
    logsys::get()->warn("LLVM Module is empty!");

  // Print content.
  module->print(llvm::outs(), nullptr);
}

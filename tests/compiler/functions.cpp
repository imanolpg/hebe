#include <cstdio>
#include <gtest/gtest.h>
#include <llvm-21/llvm/IR/Constant.h>
#include <llvm-21/llvm/IR/Constants.h>
#include <llvm-21/llvm/IR/InstrTypes.h>
#include <llvm-21/llvm/IR/Instruction.h>
#include <llvm-21/llvm/IR/Type.h>
#include <llvm-21/llvm/Support/Casting.h>
#include <llvm/IR/Value.h>
#include <regex>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

#include "ast/ast.h"
#include "compiler.h"
#include "logging.h"

TEST(Compiler, create_compiler_object) {
  Compiler c = Compiler();
  EXPECT_NE(&c, nullptr);
}

TEST(Compiler, create_compiler_oject_with_rootNode) {
  ASTNode rootNode = ASTNode(NodeType::Program);
  Compiler c = Compiler(&rootNode);
  EXPECT_NE(&c, nullptr);
}

TEST(Compiler, call_generateCode_with_no_rootNode) {
  Compiler c = Compiler();
  EXPECT_THROW(c.generateCode(), std::runtime_error);
}

TEST(Compiler, print_node_tree_with_no_nodes) {
  Compiler c = Compiler();
  EXPECT_THROW(c.printNodeTree(), std::runtime_error);
}

TEST(Compiler, print_node_tree_with_number_node) {

  // Clear the logger sinks just in case.
  logsys::get()->sinks().clear();

  // Create a sink with a pattern so that the logger can print.
  auto captured_oss = std::make_shared<std::ostringstream>();
  auto test_sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(*captured_oss);
  test_sink->set_pattern(logsys::loginPattern);

  // Add the sink to the logger.
  logsys::get()->sinks().push_back(test_sink);

  // Ensure level is set to capture the info calls
  logsys::get()->set_level(spdlog::level::info);

  // Begin the actual testing.
  NumberNode num(0.0);
  Compiler c(&num);
  c.printNodeTree(&num);

  std::regex pattern("\\[\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d{3}\\] "
                     "\\[info\\](\\s*|\t+)Number: \\d+.?\\d*\n?");

  // Extract the captured string.
  std::string consoleOutput = captured_oss->str();

  // Clear again the sinks.
  logsys::get()->sinks().clear();

  ASSERT_FALSE(consoleOutput.empty()) << "Logger captured nothing!";
  EXPECT_TRUE(std::regex_search(consoleOutput, pattern));
}

TEST(Compiler_codegen, number_node) {

  // Create a list of float values to test.
  float testValues[5] = {-100000.121212, -1.0f, 0.0f, 10.0f, 123423421.23f};

  for (float testValue : testValues) {

    NumberNode numberNode = NumberNode(testValue);
    ASTNode* numberNodeAST = static_cast<ASTNode*>(&numberNode);

    Compiler c = Compiler();
    llvm::Value* expression = c.codegenExpr(numberNodeAST);

    // The expression should be of type Float.
    EXPECT_TRUE(expression->getType()->isFloatTy());

    // Check that the value of the returned expression matches the one originally used.
    llvm::ConstantFP* constantFloatPointExpression = llvm::dyn_cast<llvm::ConstantFP>(expression);
    float floatValue = constantFloatPointExpression->getValueAPF().convertToFloat();

    EXPECT_FLOAT_EQ(floatValue, testValue);
  }
}

TEST(Compiler_codegen, binary_op_node) {
  // Create lists for the values to test (operations and numeric).
  char opList[4] = {'+', '-', '*', '/'};
  float testValues[5] = {-100000.121212, -1.0f, 0.0f, 10.0f, 123423421.23f};

  // Test each operator will all number combinations.
  for (char op : opList) {
    for (int x = 0; x < sizeof(testValues) / sizeof(float); x++) {
      for (int y = x + 1; y < sizeof(testValues) / sizeof(float); y++) {

        // Create the binary operation node and all the nested nodes required.
        NumberNode* leftNodeAST = new NumberNode(testValues[x]);
        NumberNode* rightNodeAST = new NumberNode(testValues[y]);

        BinaryOpNode binOpNode = BinaryOpNode(op, leftNodeAST, rightNodeAST);
        ASTNode* binOpNodeAST = static_cast<ASTNode*>(&binOpNode);

        Compiler c = Compiler();

        // Evaluate node.
        llvm::Value* expression = c.codegenExpr(binOpNodeAST);

        // Ensure the expression was correctly generated.
        ASSERT_NE(expression, nullptr);

        // Ensure the expression returns a float type.
        EXPECT_TRUE(expression->getType()->isFloatTy());

        // Ensure the expression is a binary operation.
        EXPECT_TRUE(llvm::isa<llvm::BinaryOperator>(expression));

        // Transform the expression to a instruction to check the correct mapping of the instruction
        // and the operation code.
        llvm::Instruction* inst = llvm::dyn_cast<llvm::Instruction>(expression);
        ASSERT_NE(inst, nullptr);

        switch (inst->getOpcode()) {
        case llvm::Instruction::FAdd:
          ASSERT_EQ(op, '+');
          break;
        case llvm::Instruction::FSub:
          ASSERT_EQ(op, '-');
          break;
        case llvm::Instruction::FMul:
          ASSERT_EQ(op, '*');
          break;
        case llvm::Instruction::FDiv:
          ASSERT_EQ(op, '/');
          break;
        default:
          FAIL() << "Not known operation character.";
        }
      }
    }
  }
}

TEST(Compiler_codegen, assignment_node) {

  std::string variableName = "varName";
  NumberNode* numberNodeAST = new NumberNode(0.0);
  AssignmentNode* assignmentNodeAST = new AssignmentNode(variableName, numberNodeAST);
  ProgramNode* programNode = new ProgramNode();
  programNode->append(assignmentNodeAST);

  Compiler c = Compiler(programNode);
  c.generateCode();

  SUCCEED();
}

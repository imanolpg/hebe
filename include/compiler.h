#pragma once

#include <llvm-21/llvm/IR/NoFolder.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <memory>
#include <string>
#include <unordered_map>

#include "ast/ast.h"

class Compiler {
public:
  // ================================================================================================
  // Constructor and destructor

  Compiler() { this->initializeLLVM(); }

  Compiler(ASTNode* rootNode) {
    this->initializeLLVM();
    this->rootNode = rootNode;
  };

  ~Compiler() = default;

  // =================================================================================================

  void printNodeTree(ASTNode* node = nullptr, int depth = 0);
  void printLLVMIR();
  void exportIRToFile(const std::string& fileName = "output.ll");

  void generateCode();

  int runJIT();

  // =================================================================================================
  // Testing classes that will have access to private attributes and methods.

  friend class Parsing_number_node_parsing_Test;
  friend class Compiler_codegen_number_node_Test;
  friend class Compiler_codegen_binary_op_node_Test;
  friend class Compiler_codegen_assignment_node_Test;

private:
  std::unique_ptr<llvm::LLVMContext> context;

#ifdef HEBE_TESTS_ENABLED
  std::unique_ptr<llvm::IRBuilder<llvm::NoFolder>> builder;
#else
  std::unique_ptr<llvm::IRBuilder<>> builder;
#endif

  std::unique_ptr<llvm::Module> module;

  // Node where Bison will save a ProgramNode with all the parsed AST.
  ASTNode* rootNode = nullptr;

  // ===============================================================================================
  // Lookup tables

  // Table containing name <std::string> and pointer <llvm::Function*> to all created functions.
  std::unordered_map<std::string, llvm::Function*> functionTable;
  // Table containing name <std::string> and pointer <llvm::BasicBlock*> to all created basic
  // blocks.
  std::unordered_map<std::string, llvm::BasicBlock*> basicBlockTable;
  // Table containing name <std::string> and pointer <llvm::GlobalVariable*> to all created global
  // variables.
  std::unordered_map<std::string, llvm::GlobalVariable*> globalVariableTable;

  // ================================================================================================

  void initializeLLVM();

  llvm::Value* codegenExpr(ASTNode* node);
  llvm::Value* codegenNumber(ASTNode* inputNode);
  llvm::Value* codegenBinaryOp(ASTNode* inputNode);
  llvm::Value* codegenAssignment(ASTNode* inputNode);
  llvm::Value* codegenProcedureBody(ASTNode* inputNode);
  llvm::Value* codegenProcedure(ASTNode* inputNode);
  llvm::Value* codegenProcedureCall(ASTNode* inputNode);

  llvm::Function* createFunction(const std::string& name, llvm::FunctionType* type);
  llvm::Function* getFunction(const std::string& name);
  llvm::Function* getOrCreateFunction(const std::string& name, llvm::FunctionType* type);
  llvm::FunctionType* createFunctionType(llvm::Type* result);
  llvm::BasicBlock* createBasicBlock(const std::string& name,
                                     const std::string& parentFunctionName);
  llvm::BasicBlock* getBasicBlock(const std::string& name);

  llvm::GlobalVariable* createGlobalVariable(const std::string& name);
  llvm::GlobalVariable* getGlobalVariable(const std::string& name);
  llvm::GlobalVariable* getOrCreateGlobalVariable(const std::string& name);
};

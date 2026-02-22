#include "compiler.h"

#include <llvm/ExecutionEngine/Orc/ExecutionUtils.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/NoFolder.h>
#include <llvm/Support/Errc.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <stdexcept>

#include "ast/ast.h"
#include "logging.h"

void Compiler::initializeLLVM() {
  this->context = std::make_unique<llvm::LLVMContext>();

#ifdef HEBE_TESTS_ENABLED
  this->builder = std::make_unique<llvm::IRBuilder<llvm::NoFolder>>(*context);
#else
  this->builder = std::make_unique<llvm::IRBuilder<>>(*context);
#endif

  this->module = std::make_unique<llvm::Module>("MainModule", *context);
}

void Compiler::printNodeTree(ASTNode* node, int depth) {
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

void Compiler::printLLVMIR() {
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

void Compiler::exportIRToFile(const std::string& fileName) {
  std::error_code EC;
  llvm::raw_fd_ostream dest(fileName, EC, llvm::sys::fs::OF_None);

  if (EC) {
    logsys::get()->error("Could not open file {}: {}", fileName, EC.message());
    throw std::runtime_error("Error exporting .ll file");
  }

  module->print(dest, nullptr);
}

llvm::FunctionType* Compiler::createFunctionType(llvm::Type* resultType) {
  return llvm::FunctionType::get(resultType, false);
}

llvm::Function* Compiler::createFunction(const std::string& name, llvm::FunctionType* type) {

  // Check that the function does not exist.
  auto it = this->functionTable.find(name);
  if (it != this->functionTable.end()) {
    // Function already exists.
    logsys::get()->error("Function {} already exists and can not be created", name);
    throw std::runtime_error("Function already exists and can not be created");
  }

  // Create the function.
  llvm::Function* funcPtr =
      llvm::Function::Create(type, llvm::Function::ExternalLinkage, name, this->module.get());

  // Store the reference pointer into the function map.
  this->functionTable[name] = funcPtr;

  return funcPtr;
}

llvm::Function* Compiler::getFunction(const std::string& name) {
  auto it = this->functionTable.find(name);

  // Throw an error if the function has not been found.
  if (it == this->functionTable.end()) {
    logsys::get()->error("Function {} not found.", name);
    throw std::runtime_error("Function not found.");
  }

  return it->second;
}

llvm::Function* Compiler::getOrCreateFunction(const std::string& name, llvm::FunctionType* type) {
  // Return the function if exists or create a new one and return it.
  return this->functionTable.find(name) == this->functionTable.end()
             ? this->createFunction(name, type)
             : this->functionTable[name];
}

llvm::BasicBlock* Compiler::createBasicBlock(const std::string& name,
                                             const std::string& parentFunctionName) {
  // Get the pointer to the parent function.
  llvm::Function* parentFunctionPtr = this->getFunction(parentFunctionName);

  // Check if the parent function has been correctly received.
  if (!parentFunctionPtr) {
    logsys::get()->error("Parent function {} not found when getting it", parentFunctionName);
    throw std::runtime_error("Parent function not found");
  }

  // Create the basic block.
  llvm::BasicBlock* block = llvm::BasicBlock::Create(*this->context, name, parentFunctionPtr);

  // Store the basic block pointer.
  this->basicBlockTable[name] = block;

  return block;
}

llvm::GlobalVariable* Compiler::createGlobalVariable(const std::string& name) {

  // Check if the global variable exist.
  auto it = this->globalVariableTable.find(name);
  if (it != this->globalVariableTable.end()) {
    // Variable exists.
    logsys::get()->error("Variable {} already exists and can not be created", name);
    throw std::runtime_error("Variable already exists and can not be created");
  }

  // Create the global variable
  llvm::GlobalVariable* globalVarPtr = new llvm::GlobalVariable(
      *this->module, llvm::Type::getFloatTy(*this->context), false,
      llvm::GlobalValue::ExternalLinkage,
      llvm::ConstantFP::get(llvm::Type::getFloatTy(*this->context), 0.0), name);

  // Save the global variable into the global variable table.
  this->globalVariableTable[name] = globalVarPtr;

  return globalVarPtr;
}

llvm::GlobalVariable* Compiler::getGlobalVariable(const std::string& name) {
  auto it = this->globalVariableTable.find(name);

  // Throw an error if the variable has not been found.
  if (it == this->globalVariableTable.end()) {
    logsys::get()->error("Global variable {} not found.", name);
    throw std::runtime_error("Global variable not found.");
  }

  return it->second;
}

llvm::GlobalVariable* Compiler::getOrCreateGlobalVariable(const std::string& name) {
  // Return the global variable if exists or create a new one and return it.
  return this->globalVariableTable.find(name) == this->globalVariableTable.end()
             ? this->createGlobalVariable(name)
             : this->globalVariableTable[name];
}

llvm::Value* Compiler::codegenNumber(ASTNode* inputNode) {
  NumberNode* node = static_cast<NumberNode*>(inputNode);
  return llvm::ConstantFP::get(llvm::Type::getFloatTy(*this->context), node->value);
}

llvm::Value* Compiler::codegenBinaryOp(ASTNode* inputNode) {
  BinaryOpNode* node = static_cast<BinaryOpNode*>(inputNode);

  llvm::Value* leftExpr = this->codegenExpr(node->left);
  llvm::Value* rightExpr = this->codegenExpr(node->right);

  switch (node->op) {
  case '+':
    return builder->CreateFAdd(leftExpr, rightExpr, "addtmp");
  case '-':
    return builder->CreateFSub(leftExpr, rightExpr, "subtmp");
  case '*':
    return builder->CreateFMul(leftExpr, rightExpr, "multmp");
  case '/':
    return builder->CreateFDiv(leftExpr, rightExpr, "divtmp");
  default:
    logsys::get()->error("Operation '{}' not supported", node->op);
    return nullptr;
  }
}

llvm::Value* Compiler::codegenAssignment(ASTNode* inputNode) {
  AssignmentNode* node = static_cast<AssignmentNode*>(inputNode);

  // Get the global variable pointer.
  llvm::Constant* variablePtr = this->getOrCreateGlobalVariable(node->name);

  // Parse the possible expression of the variable value.
  llvm::Value* variableValue = this->codegenExpr(node->value);

  // Store the value in the variable.
  builder->CreateStore(variableValue, variablePtr);

  return variablePtr; // FIXME: a type of Value* should be returned and now is returning
                      // llvm::Constant*
}

llvm::Value* Compiler::codegenProcedureBody(ASTNode* inputNode) {
  ProcedureBodyNode* node = static_cast<ProcedureBodyNode*>(inputNode);

  // Create variable last alloca to avoid lose of instructions after optimization passes.
  // FIXME: this has to be removed. I only put this for debugging because the IR optimization would
  // remove all expressions that had a result without being stored.
  llvm::AllocaInst* lastAlloca =
      this->builder->CreateAlloca(llvm::Type::getFloatTy(*this->context), nullptr, "forDebug");

  llvm::Value* expr = nullptr;

  for (ASTNode* child : node->getItems()) {
    expr = this->codegenExpr(child);

    // FIXME: remove this, only for debugging. Storing instructions already stores desired
    // variables.
    if (child->type == NodeType::Number || child->type == NodeType::BinaryOp)
      this->builder->CreateStore(expr, lastAlloca);
  }

  return nullptr;
}

llvm::Value* Compiler::codegenProcedure(ASTNode* inputNode) {

  ProcedureNode* node = static_cast<ProcedureNode*>(inputNode);

  // Get the function type.
  llvm::FunctionType* fnTy = this->createFunctionType(llvm::Type::getVoidTy(*this->context));

  // Create the function.
  llvm::Function* fnPtr = this->createFunction(node->name, fnTy);

  // Get a new basic block for the new function.
  llvm::BasicBlock* bbPtr = this->createBasicBlock("entry", node->name);

  // Save the basic block where the builder was inserting instructions to restore it later.
  llvm::BasicBlock* oldBbPtr = this->builder->GetInsertBlock();

  // Set function insert point.
  this->builder->SetInsertPoint(bbPtr);

  // Parse the child instructions.
  this->codegenProcedureBody(node->body);

  // Return void as the function is void type.
  this->builder->CreateRetVoid();

  // Restore the old basic block to continue inserting instructions in the parent function.
  this->builder->SetInsertPoint(oldBbPtr);

  return fnPtr;
}

llvm::Value* Compiler::codegenProcedureCall(ASTNode* inputNode) {
  ProcedureCallNode* node = static_cast<ProcedureCallNode*>(inputNode);

  // Get the procedure pointer to create a call instruction.
  llvm::Function* procPtr = this->module->getFunction(node->name);

  if (!procPtr) {
    logsys::get()->error("Function {} not found in llvm module", node->name);
    throw std::runtime_error("Function not found in llvm module");
  }

  // Call the procedure.
  llvm::Value* retVal = this->builder->CreateCall(procPtr);

  return retVal;
}

llvm::Value* Compiler::codegenExpr(ASTNode* node) {
  if (!node)
    return nullptr;
  switch (node->type) {
  case NodeType::Number:
    return this->codegenNumber(node);
  case NodeType::BinaryOp:
    return this->codegenBinaryOp(node);
  case NodeType::Assignment:
    return this->codegenAssignment(node);
  case NodeType::ProcedureBody:
    return this->codegenProcedureBody(node);
  case NodeType::Procedure:
    return this->codegenProcedure(node);
  case NodeType::ProcedureCall:
    return this->codegenProcedureCall(node);
  default:
    logsys::get()->error("Code generation for type {} not supported.", getNodeType(node->type));
    throw std::runtime_error("Code generation for this type of node not supported");
  }
}

void Compiler::generateCode() {
  logsys::get()->info("Executing generateCode");

  // Ensure there is a list of nodes.
  if (!this->rootNode) {
    logsys::get()->error("No code provided. rootNode is empty");
    throw std::runtime_error("Failed to generate code.");
  }

  // Create main function where the code will run.
  llvm::FunctionType* mainFuncTy = this->createFunctionType(llvm::Type::getFloatTy(*this->context));
  this->getOrCreateFunction("run", mainFuncTy);

  // Create the basic block that will be executed on program start.
  llvm::BasicBlock* entry = this->createBasicBlock("entry", "run");
  this->builder->SetInsertPoint(entry);

  // Create variable last alloca to avoid lose of instructions after optimization passes.
  // FIXME: this has to be removed. I only put this for debugging because the IR optimization would
  // remove all expressions that had a result without being stored.
  llvm::AllocaInst* lastAlloca =
      this->builder->CreateAlloca(llvm::Type::getFloatTy(*this->context), nullptr, "forDebug");

  llvm::Value* expr = nullptr;

  ProgramNode* program = static_cast<ProgramNode*>(this->rootNode);
  for (ASTNode* node : program->getItems()) {
    expr = this->codegenExpr(node);

    // FIXME: remove this, only for debugging. Storing instructions already stores desired
    // variables.
    if (node->type == NodeType::Number || node->type == NodeType::BinaryOp)
      this->builder->CreateStore(expr, lastAlloca);
  }

  if (expr) {
    llvm::Constant* retPtr = this->getOrCreateGlobalVariable("ret");
    llvm::LoadInst* retValue =
        this->builder->CreateLoad(llvm::Type::getFloatTy(*this->context), retPtr);
    this->builder->CreateRet(retValue);
  } else {
    logsys::get()->error("Failed to generate code. Last evaluated expression has an error.");
    throw std::runtime_error("Failed to generate code. Last evaluated expression has an error.");
  }
}

int Compiler::runJIT() {
  // Init LLVM JIT target.
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();

  // Create the JIT.
  auto jitExpected = llvm::orc::LLJITBuilder().create();
  if (!jitExpected) {
    llvm::errs() << "Failed to create LLJIT\n";
    return 1;
  }
  auto J = std::move(*jitExpected);

  // Allow JITed code to resolve symbols from the current process.
  auto genExpected = llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(
      J->getDataLayout().getGlobalPrefix());
  if (!genExpected) {
    llvm::errs() << "Failed to create symbol generator\n";
    return 1;
  }
  J->getMainJITDylib().addGenerator(std::move(*genExpected));

  // Put your Module into a ThreadSafeModule and add it to the JIT.
  llvm::orc::ThreadSafeModule TSM(std::move(this->module), std::move(this->context));
  if (auto err = J->addIRModule(std::move(TSM))) {
    llvm::errs() << "Failed to add IR module to JIT\n";
    return 1;
  }

  // Look up the entry function in the JIT "run".
  auto symExpected = J->lookup("run");
  if (!symExpected) {
    llvm::errs() << "Could not find function 'run' in JIT\n";
    return 1;
  }

  // Call it like a normal C function.
  using RunFn = float (*)();
  auto addr = symExpected->getValue();
  auto runFn = reinterpret_cast<RunFn>(addr);
  float result = runFn();

  return static_cast<int>(result);
}
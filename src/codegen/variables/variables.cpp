#include "codegen/codegen.h"

#include "logging.h"

llvm::GlobalVariable* CodeGen::createGlobalVariable(const std::string& name) {

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

llvm::GlobalVariable* CodeGen::getGlobalVariable(const std::string& name) {
  auto it = this->globalVariableTable.find(name);

  // Throw an error if the variable has not been found.
  if (it == this->globalVariableTable.end()) {
    logsys::get()->error("Global variable {} not found.", name);
    throw std::runtime_error("Global variable not found.");
  }

  return it->second;
}

llvm::GlobalVariable* CodeGen::getOrCreateGlobalVariable(const std::string& name) {
  // Return the global variable if exists or create a new one and return it.
  return this->globalVariableTable.find(name) == this->globalVariableTable.end()
             ? this->createGlobalVariable(name)
             : this->globalVariableTable[name];
}

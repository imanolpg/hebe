#include "codegen/codegen.h"

#include "logging.h"

llvm::FunctionType* CodeGen::createFunctionType(llvm::Type* resultType) {
  return llvm::FunctionType::get(resultType, false);
}

llvm::Function* CodeGen::createFunction(const std::string& name, llvm::FunctionType* type) {

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

llvm::Function* CodeGen::getFunction(const std::string& name) {
  auto it = this->functionTable.find(name);

  // Throw an error if the function has not been found.
  if (it == this->functionTable.end()) {
    logsys::get()->error("Function {} not found.", name);
    throw std::runtime_error("Function not found.");
  }

  return it->second;
}

llvm::Function* CodeGen::getOrCreateFunction(const std::string& name, llvm::FunctionType* type) {
  // Return the function if exists or create a new one and return it.
  return this->functionTable.find(name) == this->functionTable.end()
             ? this->createFunction(name, type)
             : this->functionTable[name];
}
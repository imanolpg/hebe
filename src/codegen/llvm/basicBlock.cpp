#include "codegen/codegen.h"

#include "logging.h"

llvm::BasicBlock* CodeGen::createBasicBlock(const std::string& name,
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
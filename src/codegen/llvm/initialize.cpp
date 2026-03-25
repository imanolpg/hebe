#include "codegen/codegen.h"

void CodeGen::initializeLLVM() {
  this->context = std::make_unique<llvm::LLVMContext>();

#ifdef HEBE_TESTS_ENABLED
  this->builder = std::make_unique<llvm::IRBuilder<llvm::NoFolder>>(*context);
#else
  this->builder = std::make_unique<llvm::IRBuilder<>>(*context);
#endif

  this->module = std::make_unique<llvm::Module>("MainModule", *context);
}
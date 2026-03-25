#include "codegen/codegen.h"

#include <llvm/Support/FileSystem.h>

#include "logging.h"

void CodeGen::exportIRToFile(const std::string& fileName) {
  std::error_code EC;
  llvm::raw_fd_ostream dest(fileName, EC, llvm::sys::fs::OF_None);

  if (EC) {
    logsys::get()->error("Could not open file {}: {}", fileName, EC.message());
    throw std::runtime_error("Error exporting .ll file");
  }

  module->print(dest, nullptr);
}

#include "codegen/codegen.h"

#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/Support/TargetSelect.h>

int CodeGen::runJIT() {
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
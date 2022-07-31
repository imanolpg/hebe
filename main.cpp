
#include "src/Parser.h"
#include "src/JITDriver.h"
#include "src/CodeGeneration.h"


int main(int argc, char **argv) {

    InitializeModule();

    MainLoop();

    // print the generated code
    TheModule->print(llvm::errs(), nullptr);

    return 0;
}


#include <iostream>
#include <memory>
#include <vector>

#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/IR/Module.h"

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/DiagnosticIDs.h"

#include "clang/CodeGen/CodeGenAction.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"

namespace vg {
clang::EmitLLVMOnlyAction* getAction(int, const char**);
}

using namespace llvm;

int main(int argc, const char** argv) {

  OwningPtr<clang::EmitLLVMOnlyAction> action(vg::getAction(argc, argv));

  Module* module = action->takeModule();
  for (Function& function : module->getFunctionList()) {
    std::cout << function.getName().str() << std::endl;
    auto& list = function.front().getInstList();
    for (auto inst = list.begin(); inst != list.end(); inst++) {
      std::cout << inst->getOpcodeName() << std::endl;
    }
  }

  return 0;
}

namespace vg {

using namespace clang;

EmitLLVMOnlyAction*
getAction(int argc, const char** argv) {

  std::vector<const char*> args(argv, argv + argc);

  llvm::IntrusiveRefCntPtr<DiagnosticIDs> diagIDs(new DiagnosticIDs);
  DiagnosticsEngine diagEngine(diagIDs, new DiagnosticOptions());

  llvm::OwningPtr<CompilerInvocation> CI(new CompilerInvocation);
  CompilerInvocation::CreateFromArgs(*CI, &args[1], &args[0] + args.size(),
                                     diagEngine);

  CompilerInstance instance;
  instance.setInvocation(CI.take());
  auto diags = instance.createDiagnostics(new DiagnosticOptions());
  //diags->setSuppressAllDiagnostics(true);
  instance.setDiagnostics(diags.getPtr());
  if (!instance.hasDiagnostics()) {
    std::cerr << "Compiler instance has no diagnostics" << std::endl;
    return nullptr;
  }

  EmitLLVMOnlyAction* action = new EmitLLVMOnlyAction();
  if (!instance.ExecuteAction(*action)) {
    std::cerr << "Action execution failed" << std::endl;
    return nullptr;
  }

  return action;
}

} // namespace vg

#include <iostream>
#include <memory>
#include <vector>

#include "llvm/Support/CommandLine.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/IR/Module.h"

#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"

#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/CodeGen/CodeGenAction.h"

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/DiagnosticIDs.h"

using namespace clang::tooling;
using namespace clang;
using namespace llvm;
using namespace std;


int main(int argc, const char **argv) {

  string inPath = "inputs/test.c";
  vector<const char*> args;
  args.push_back(inPath.c_str());

  IntrusiveRefCntPtr<DiagnosticIDs> diagIDs(new DiagnosticIDs);
  DiagnosticsEngine diagEngine(diagIDs, new DiagnosticOptions());

  OwningPtr<CompilerInvocation> CI(new CompilerInvocation);
  CompilerInvocation::CreateFromArgs(*CI, &args[0], &args[0] + args.size(),
                                     diagEngine);

  CompilerInstance instance;
  instance.setInvocation(CI.take());

  instance.setDiagnostics(instance.createDiagnostics(new DiagnosticOptions()).getPtr());
  if (!instance.hasDiagnostics()) {
    cout << "no diagnostics" << endl;
    return 1;
  }

  OwningPtr<CodeGenAction> action(new EmitLLVMOnlyAction());
  if (!instance.ExecuteAction(*action)) {
    cout << "execute action failed" << endl;
    return 1;
  }

  llvm::Module* module = action->takeModule();
  for (llvm::Function& function : module->getFunctionList())
    cout << function.getName().str() << endl;

  return 0;
}

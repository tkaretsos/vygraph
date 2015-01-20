#include <iostream>

#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

#include "Analysis/Analyzer.hh"
#include "Analysis/FunctionManager.hh"
#include "Inline/InlineAction.hh"
#include "Translate/TranslateAction.hh"
#include "Utility/CLOptions.hh"

using namespace clang::tooling;
using namespace llvm;
using namespace std;
using namespace vy;

int main(int argc, const char **argv) {
  cl::OptionCategory vygraphCategory("vygraph options");
  auto& clOptions = CLOptions::Instance();
  clOptions.toolActionBits.reset(new cl::bits<CLOptions::ToolActionBit>(
    cl::desc("Choose a specific action to perform (optional):"),
    cl::values(
      clEnumValN(CLOptions::InlineOnly, "inline-only",
                 "Only perform C inlining"),
      clEnumValN(CLOptions::TranslateOnly, "translate-only",
                 "Only perform translation from C to Multimono"),
      clEnumValEnd
    ),
    cl::cat(vygraphCategory)
  ));

  CommonOptionsParser optParser(argc, argv);

  if (clOptions.inlineOnly() && clOptions.translateOnly()) {
    cout << R"(Error: You cannot use both "-inline-only" and "-translate-only" options.)";
    cout << endl << "Exiting..." << endl;

    return 1;
  }

  ClangTool tool(optParser.getCompilations(), optParser.getSourcePathList());
  Analyzer analyzer(tool);
  analyzer.analyze();

  if (!clOptions.translateOnly())
    tool.run(newFrontendActionFactory<InlineAction>());
  if (!clOptions.inlineOnly())
    tool.run(newFrontendActionFactory<TranslateAction>());

  return 0;
}

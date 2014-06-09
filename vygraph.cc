#include <iostream>

#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
// Declares llvm::cl::extrahelp.
#include "llvm/Support/CommandLine.h"

#include "Inline/InlineAction.hh"

#include "Analysis/FunctionManager.hh"
#include "Analysis/Analyzer.hh"

using namespace clang::tooling;
using namespace llvm::cl;

using namespace clang;
using namespace clang::ast_matchers;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static OptionCategory MyToolCategory("vygraph options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static extrahelp MoreHelp("\nMore help text...");

int main(int argc, const char **argv) {
  CommonOptionsParser optionsParser(argc, argv);
  ClangTool tool(optionsParser.getCompilations(),
                 optionsParser.getSourcePathList());

  vy::Analyzer analyzer(tool);
  analyzer.analyze();

  tool.run(newFrontendActionFactory<vy::InlineAction>());
  return 0;
}

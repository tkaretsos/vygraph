#include "InlineAction.hh"

#include "clang/Frontend/CompilerInstance.h"

#include "Analysis/FunctionManager.hh"
#include "InlineASTConsumer.hh"
#include "Utility/CLOptions.hh"

namespace vy {

using namespace clang;

ASTConsumer*
InlineAction::CreateASTConsumer(CompilerInstance& CI, llvm::StringRef file) {
  rewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
  return new InlineASTConsumer(rewriter, CI.getASTContext());
}

void
InlineAction::EndSourceFileAction() {
  if (CLOptions::Instance().inlineOnly())
    writeToFile();
  else
    writeToMemory();
}

void
InlineAction::writeToFile() {
  auto& SM = rewriter.getSourceMgr();
  std::string filename = SM.getFileEntryForID(SM.getMainFileID())->getName();
  size_t found = filename.find_last_of('.');
  if (found != std::string::npos)
    filename.insert(found, ".inlined");
  else
    filename.append(".inlined.c");
  std::string errors;
  llvm::raw_fd_ostream rfo(filename.c_str(), errors);
  if (!errors.empty())
    llvm::errs() << errors.c_str() << "\n";
  else
    rewriter.getEditBuffer(SM.getMainFileID()).write(rfo);
}

void
InlineAction::writeToMemory() {
  auto& SM = rewriter.getSourceMgr();
  llvm::raw_string_ostream rso(functionMgr.fileContents);
  rewriter.getEditBuffer(SM.getMainFileID()).write(rso);
  rso.flush();
}

} // namespace vy

/** @file */

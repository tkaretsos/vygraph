#include "InlineAction.hh"

#include "clang/Frontend/CompilerInstance.h"

#include "InlineASTConsumer.hh"

namespace vy {

using namespace clang;

InlineAction::InlineAction() { }

ASTConsumer* InlineAction::CreateASTConsumer(CompilerInstance& CI,
                                             llvm::StringRef file) {
  rewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
  return new InlineASTConsumer(rewriter, CI.getASTContext());
}

void InlineAction::EndSourceFileAction() {
  SourceManager& SM = rewriter.getSourceMgr();
  std::string filename = SM.getFileEntryForID(SM.getMainFileID())->getName();
  size_t found = filename.find_last_of('.');
  if (found != std::string::npos)
    filename.insert(found, ".vy");
  else
    filename.append(".vy.c");
  std::string errors;
  llvm::raw_fd_ostream outfile(filename.c_str(), errors);
  if (!errors.empty())
    llvm::errs() << errors.c_str() << "\n";
  else {
    rewriter.getEditBuffer(SM.getMainFileID()).write(outfile);
    rewriter.getEditBuffer(SM.getMainFileID()).write(llvm::outs());
  }
}

} // namespace vy

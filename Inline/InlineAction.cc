#include "InlineAction.hh"

#include "clang/Frontend/CompilerInstance.h"

#include "InlineASTConsumer.hh"

namespace vy {

using namespace clang;

ASTConsumer*
InlineAction::CreateASTConsumer(CompilerInstance& CI, llvm::StringRef file) {
  rewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
  return new InlineASTConsumer(rewriter, CI.getASTContext());
}

void
InlineAction::EndSourceFileAction() {
  auto& SM = rewriter.getSourceMgr();
  std::string filename = SM.getFileEntryForID(SM.getMainFileID())->getName();
  size_t found = filename.find_last_of('.');
  if (found != std::string::npos)
    filename.insert(found, ".inlined");
  else
    filename.append(".inlined.c");
  std::string errors;
  llvm::raw_fd_ostream outfile(filename.c_str(), errors);
  if (!errors.empty())
    llvm::errs() << errors.c_str() << "\n";
  else {
    rewriter.getEditBuffer(SM.getMainFileID()).write(outfile);
//     rewriter.getEditBuffer(SM.getMainFileID()).write(llvm::outs());
  }
}

} // namespace vy

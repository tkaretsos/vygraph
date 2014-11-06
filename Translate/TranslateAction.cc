#include "TranslateAction.hh"

#include <fstream>

#include "clang/Frontend/CompilerInstance.h"

#include "TranslateASTConsumer.hh"

namespace vy {

using namespace clang;

TranslateAction::TranslateAction() { }

ASTConsumer*
TranslateAction::CreateASTConsumer(CompilerInstance& CI, llvm::StringRef file) {
  return new TranslateASTConsumer(CI.getASTContext(), outStream);
}

bool
TranslateAction::BeginSourceFileAction(CompilerInstance& CI, StringRef filename) {
  outFileName = filename.str();
  auto found = outFileName.find_last_of('.');
  if (found != std::string::npos)
    outFileName.replace(outFileName.begin() + found, outFileName.end(), ".vyc");
  else
    outFileName.append(".vyc");
  return true;
}

void
TranslateAction::EndSourceFileAction() {
  std::ofstream outFile;
  outFile.open(outFileName, std::ios_base::trunc);
  outFile << outStream.rdbuf();
  outFile.flush();
  outFile.close();
}


} // namespace vy

#include "TranslateAction.hh"

#include <fstream>

#include "clang/Frontend/CompilerInstance.h"
#include "llvm/Support/MemoryBuffer.h"

#include "TranslateASTConsumer.hh"
#include "Analysis/FunctionManager.hh"
#include "Utility/CLOptions.hh"

namespace vy {

using namespace clang;

ASTConsumer*
TranslateAction::CreateASTConsumer(CompilerInstance& CI, StringRef file) {
  if (!CLOptions::Instance().translateOnly()) {
    auto& SM = CI.getSourceManager();
    auto memBuffer = llvm::MemoryBuffer::getMemBuffer(functionMgr.fileContents);
    auto fid = SM.createFileIDForMemBuffer(memBuffer);
    SM.setMainFileID(fid);
  }

  return new TranslateASTConsumer(CI.getASTContext(), outStream);
}

bool
TranslateAction::BeginSourceFileAction(CompilerInstance& CI, StringRef filename) {
  outFileName = filename.str();
  auto found = outFileName.find_last_of('.');
  if (found != std::string::npos)
    outFileName.replace(outFileName.begin() + found, outFileName.end(), ".mmn");
  else
    outFileName.append(".mmn");

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

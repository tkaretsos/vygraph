set(CMAKE_CXX_FLAGS "-std=c++11")

add_clang_executable(vygraph
  Analysis/FunctionManager.cc
  Analysis/Analyzer.cc
  Inline/InlineASTVisitor.cc
  Inline/InlineASTConsumer.cc
  Inline/InlineAction.cc
  Inline/Inliner.cc
  Translate/CFGAnalyzer.cc
  Translate/TranslateAction.cc
  Translate/TranslateASTConsumer.cc
  Translate/Translator.cc
  Utility/Functions.cc
  Utility/PostDominatorTree.cc
  Utility/CLOptions.cc
  vygraph.cc
)

target_link_libraries(vygraph
  clangTooling
  clangFormat
  clangFrontend
  clangBasic
)


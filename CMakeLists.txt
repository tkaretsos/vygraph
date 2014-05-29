set(CMAKE_CXX_FLAGS "-std=c++11")

add_clang_executable(vygraph
    Inline/InlineASTVisitor.cc
    Inline/InlineASTConsumer.cc
    Inline/InlineAction.cc
    Inline/AnalysisASTVisitor.cc
    FunctionManager.cc
    vygraph.cc
)

target_link_libraries(vygraph
    clangTooling
    clangFormat
    clangFrontend
    clangBasic
)


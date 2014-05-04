set(CMAKE_CXX_FLAGS "-std=c++11")
set(LLVM_LINK_COMPONENTS support)
set(LLVM_USED_LIBS clangTooling clangBasic clangAST)

add_clang_executable(vygraph
    LoopConvert.cc
)

target_link_libraries(vygraph
    clangTooling
    clangCodeGen
    clangASTMatchers
    clangBasic
)


set(CMAKE_CXX_FLAGS "-std=c++11")

add_clang_executable(vygraph
    LoopConvert.cc
)

target_link_libraries(vygraph
    clangCodeGen
    clangFrontend
    clangBasic
)


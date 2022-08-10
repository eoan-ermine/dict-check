cmake_minimum_required(VERSION 3.16)
project(dict-check)

set(LLVM_LINK_COMPONENTS
  Support
)

add_clang_executable(dict-check FindDecls.cpp)

target_link_libraries(dict-check
  PRIVATE
  clangAST clangBasic clangFrontend
  clangSerialization clangTooling
)
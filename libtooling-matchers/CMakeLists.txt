cmake_minimum_required(VERSION 3.16)
project(dict-check-libtooling-matchers)

set(LLVM_LINK_COMPONENTS
  Support
)

add_clang_executable(dict-check-libtooling-matchers src/main.cpp)

target_link_libraries(dict-check-libtooling-matchers
  PRIVATE
  clangAST clangASTMatchers clangBasic
  clangFrontend clangSerialization clangTooling
)
cmake_minimum_required(VERSION 3.16)
project(dict-check-recursive-visitor)

set(LLVM_LINK_COMPONENTS
  Support
)

add_clang_executable(dict-check-recursive-visitor src/main.cpp)

target_link_libraries(dict-check-recursive-visitor
  PRIVATE
  clangAST clangBasic clangFrontend
  clangSerialization clangTooling
)
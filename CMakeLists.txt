cmake_minimum_required(VERSION 3.16)
project(find-decls)

set(LLVM_LINK_COMPONENTS
  Support
)

add_clang_executable(find-decls FindDecls.cpp)

target_link_libraries(find-decls
  PRIVATE
  clangAST clangBasic clangFrontend
  clangSerialization clangTooling
)

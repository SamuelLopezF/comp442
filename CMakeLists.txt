cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(lexical_analyser)

add_executable(
  lexical_analyser

  # h flies
  ./include/tokenizer.h
  ./include/filereader.h
  ./include/parser.h
  ./include/filereader.h
  ./include/ast.h
  ./include/ast_builder.h    
  # cpp files
  ./src/tokenizer.cpp 
  ./src/filereader.cpp
  ./src/parser.cpp
  ./src/ast.cpp
  ./src/ast_builder.cpp
  ./src/main.cpp)

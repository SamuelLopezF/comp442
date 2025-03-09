cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(lexical_analyser)

add_executable(
  lexical_analyser

  # h flies
  ./include/tokenizer.h
  ./include/filereader.h
  # cpp files
  ./src/tokenizer.cpp 
  ./src/filereader.cpp
  ./src/main.cpp)

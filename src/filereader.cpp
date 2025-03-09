#include "../include/filereader.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

// Constructor: Reads the file into memory
FileCharReader::FileCharReader(const std::string &filePath) {
  std::ifstream file(filePath, std::ios::in | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file: " + filePath);
  }

  // Read file content into a string
  fileContent = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  file.close();
}

// Returns a pointer to the start of the file content
const char* FileCharReader::getCharPointer() const {
  return fileContent.c_str();
}

// Returns the size of the file content
size_t FileCharReader::size() const {
  return fileContent.size();
}

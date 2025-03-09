#ifndef FILEREADER_H
#define FILEREADER_H

#include <string>

class FileCharReader {
private:
  std::string fileContent; // Holds the file content in memory

public:
  // Constructor: Reads the file into memory
  FileCharReader(const std::string &filePath);

  // Returns a pointer to the start of the file content
  const char* getCharPointer() const;

  // Returns the size of the file content
  size_t size() const;
};

#endif // FILEREADER_H

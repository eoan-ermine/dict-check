#pragma once

#include <unordered_set>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

int readSourceFile(const std::string& filename, std::string& sourceCode) {
  std::ostringstream sourceBuffer;
  std::ifstream stream(filename);
  std::string line;

  if(!stream.is_open()) {
    return 0;
  }

  while (std::getline(stream, line)) {
    sourceBuffer << line << '\n';
  }

  sourceCode = sourceBuffer.str();
  return 1;
}

int readDictionaryFile(const std::string& filename, std::unordered_set<std::string>& dictionary) {
  std::ifstream stream(filename);
  std::string word;

  if (!stream.is_open()) {
    return 0;
  }

  while (stream >> word) {
    dictionary.insert(word);
  }

  return 1;
}

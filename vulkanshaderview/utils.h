#ifndef UTILS_H
#define UTILS_H


#include <vector>
#include <string>
#include <fstream>

// read file as binary data and fill vector with bytes
std::vector<char> readFile(const std::string& filename);

#endif
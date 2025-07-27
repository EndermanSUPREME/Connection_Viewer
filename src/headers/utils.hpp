#ifndef UTILS
#define UTILS

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

std::vector<std::string> splitLine(const std::string line, char delimiter = ' ');
std::string get_line(std::ifstream& file);

#endif
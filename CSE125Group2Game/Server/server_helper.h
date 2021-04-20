#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <fstream>
#include <iostream>
#include <string>

std::string server_read_config_file(std::string field, std::string filename) {
  std::string line;
  std::ifstream infile;

  infile.open(filename.c_str());
  std::string intermediate;

  // read the config file and fill in port and tick values, if defined
  if (!infile) {
    return std::string();
  }

  while (getline(infile, line)) {
    std::stringstream ss(line);
    std::vector<std::string> tokens;

    while (getline(ss, intermediate, ':')) {
      tokens.push_back(intermediate);
    }

    std::cout << "Token: " << tokens[0] << std::endl;

    if (tokens[0].compare(field) == 0) {
      return tokens[1];
    }
  }

  infile.close();
  return std::string();
}

// std::string server_read_config_file(std::string field, std::string filename);

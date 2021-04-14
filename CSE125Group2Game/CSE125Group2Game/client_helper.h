#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <fstream>
#include <iostream>
#include <string>

#include "net_client.hpp"

bool read_client_config(std::string& host, uint16_t& port,
                        std::string filename) {
  std::string line;
  std::ifstream infile;

  infile.open(filename.c_str());
  std::vector<std::string> tokens;
  std::string intermediate;

  // read the config file and fill in port and host values, if defined
  if (!infile) {
    return false;
  }
  while (getline(infile, line)) {
    std::stringstream ss(line);

    while (getline(ss, intermediate, ':')) {
      tokens.push_back(intermediate);
    }

    if (tokens[0].compare("port") == 0) {
      int temp_int(std::stoi(tokens[1]));

      if (temp_int <= static_cast<int>(UINT16_MAX) && temp_int >= 0) {
        port = static_cast<uint16_t>(temp_int);
      }
    } else if (tokens[0].compare("host") == 0) {
      host = tokens[1];
    }
  }
}

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <fstream>
#include <iostream>
#include <string>

bool server_read_config(uint16_t& port, uint16_t& tick, std::string filename) {
  std::string line;
  std::ifstream infile;

  infile.open(filename.c_str());
  std::string intermediate;

  // read the config file and fill in port and tick values, if defined
  if (!infile) {
    return false;
  }

  while (getline(infile, line)) {
    std::stringstream ss(line);
    std::vector<std::string> tokens;

    while (getline(ss, intermediate, ':')) {
      tokens.push_back(intermediate);
    }

    std::cout << "Token: " << tokens[0] << std::endl;

    if (tokens[0].compare("port") == 0) {
      int temp_int(std::stoi(tokens[1]));

      if (temp_int <= static_cast<int>(UINT16_MAX) && temp_int >= 0) {
        port = static_cast<uint16_t>(temp_int);
      }
    } else if (tokens[0].compare("tick") == 0) {
      std::cout << "tick updated!" << std::endl;
      int temp_int(std::stoi(tokens[1]));

      if (temp_int <= static_cast<int>(UINT16_MAX) && temp_int >= 0) {
        tick = static_cast<uint16_t>(temp_int);
      }
    }
  }

  return true;
}

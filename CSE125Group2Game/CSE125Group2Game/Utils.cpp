#include "Utils.h"

void PrintCrash(const std::string& msg, const std::string& file, int line) {
  std::cerr << "Fatal Error: " << msg << std::endl;
  std::cerr << "\tin " << file << ":" << line << std::endl;
  std::cerr << "\n";
  std::cerr << "Press enter to exit..." << std::endl;

  std::cin.get();

  exit(1);
}

#include <exception>
#include <iostream>

int main() {
  try {

  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }

  return 0;
}
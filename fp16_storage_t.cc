#include <iostream>
#include <iomanip>

#include "fps/fp16_storage_t.hh"

int main() {

  using namespace fps::literals;

  std::cout << std::hex << (std::uint32_t) 1.0_fs16 << std::endl;
  std::cout << std::hex << (std::uint32_t) 0._fs16 << std::endl;
}

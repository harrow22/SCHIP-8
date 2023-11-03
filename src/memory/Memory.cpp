#include "Memory.h"
#include <fstream>

bool Memory::load(std::string path) {
    // writing the font into memory
    std::uint8_t addr {fontAddr};
    for (std::uint8_t byte : font)
        ram[addr++] = byte;

    // writing the superchip font into memory
    addr = bigFontAddr;
    for (std::uint8_t byte : schipfont)
        ram[addr++] = byte;

    std::ifstream file {path, std::ios::binary};
    if (file.fail() or !file.is_open()) return false;

    file.read(reinterpret_cast<char*>(&ram[0x200]), ram.size());
    return true;
}

#ifndef CHIP_8_MEMORY_H
#define CHIP_8_MEMORY_H

#include <cstdint>
#include <array>
#include <stack>
#include <memory>
#define MEM_SIZE 4096

class Memory {
public:
    Memory() = default;
    void wipe();
    void write(std::uint8_t byte, std::uint16_t addr) { arr.at(addr) = byte; };
    [[nodiscard]] std::uint8_t read(std::uint16_t addr) const { return arr.at(addr); };
    std::stack<std::uint16_t> stack;
private:
    std::array<std::uint8_t, MEM_SIZE> arr {};
};

#endif //CHIP_8_MEMORY_H

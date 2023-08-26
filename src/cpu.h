//
// Created by kamal on 8/25/2023.
//

#ifndef CHIP_8_CPU_H
#define CHIP_8_CPU_H

#include <cstdint>

class cpu {
public:
    cpu() = default;
    std::uint8_t fetch() const;
private:
    // registers
    std::uint16_t pc {0}; // program counter
    std::uint16_t i {0}; // index register
    std::uint8_t reg[16] {}; // general-purpose registers
    std::uint8_t dt {0}; // delay timer register
    std::uint8_t st {0}; // sound timer register
};


#endif //CHIP_8_CPU_H

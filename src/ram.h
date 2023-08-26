//
// Created by kamal on 8/25/2023.
//

#ifndef CHIP_8_RAM_H
#define CHIP_8_RAM_H


#include<cstdint>
#include<stack>

class ram {
public:
    ram() = default;
    void write(std::uint8_t);
    std::uint8_t read(std::uint16_t) const;
private:
    std::uint8_t mem[4096] {}; // 4 kb of RAM
    std::stack<std::uint16_t> stack;
};


#endif //CHIP_8_RAM_H

#ifndef CHIP_8_CPU_H
#define CHIP_8_CPU_H

#include <cstdint>
#include <utility>
#include "../Memory/Memory.h"
#include "../Display/Display.h"
#define FONT_MEMORY_LOC 0x50
#define BYTES_PER_DIGIT 5
#define DECREASE_RATE 1.0/60.0

class Cpu {
public:
    Cpu(Memory& mem, Display& dis) : memory{mem}, sp{&(mem.stack)}, display{dis},
                                     pc{pc_}, cir{cir_}, i{i_}, reg{reg_},
                                     dt{dt_}, st{st_} {};
    void cycle(double deltaT);
    void reset();

    const std::uint16_t& pc;
    const std::uint16_t& cir;
    const std::uint16_t& i;
    const std::array<std::uint8_t, 16>& reg;
    const std::uint8_t& dt;
    const std::uint8_t& st;
private:
    // fetch decode+execute
    void fetch();
    void execute();
    // instructions
    void cls();
    void ret();
    void jp(std::uint16_t);
    void call(uint16_t);
    void sei(std::uint8_t, std::uint8_t);
    void snei(std::uint8_t, std::uint8_t);
    void se(std::uint8_t, std::uint8_t);
    void ldi(std::uint8_t, std::uint8_t);
    void addi(std::uint8_t, std::uint8_t);
    void ld(std::uint8_t, std::uint8_t);
    void orr(std::uint8_t, std::uint8_t);
    void andd(std::uint8_t, std::uint8_t);
    void xorr(std::uint8_t, std::uint8_t);
    void add(std::uint8_t, std::uint8_t);
    void sub(std::uint8_t, std::uint8_t);
    void shr(std::uint8_t, std::uint8_t);
    void subn(std::uint8_t, std::uint8_t);
    void shl(std::uint8_t, std::uint8_t);
    void sne(std::uint8_t, std::uint8_t);
    void ldaddr(std::uint16_t);
    void jpr(std::uint16_t);
    void rnd(std::uint8_t, std::uint8_t);
    void drw(std::uint8_t, std::uint8_t, std::uint8_t);
    void skp(std::uint8_t);
    void sknp(std::uint8_t);
    void lddt(std::uint8_t);
    void ldkp();
    void sdt(std::uint8_t);
    void sst(std::uint8_t);
    void saddr(std::uint8_t);
    void spr(std::uint8_t);
    void sbcd(std::uint8_t);
    void sreg(std::uint8_t);
    void rd(std::uint8_t);
    // registers
    std::uint16_t pc_ {0x200}; // program counter
    std::uint16_t cir_ {0}; // current instruction register
    std::uint16_t i_ {0}; // index register
    std::array<std::uint8_t, 16> reg_ {}; // general-purpose registers
    std::uint8_t dt_ {0}; // delay timer register
    std::uint8_t st_ {0}; // sound timer register
    double timeElapsed {0};
    // stack pointer
    std::stack<std::uint16_t>* sp;
    // hardware
    Memory& memory;
    Display& display;
};


#endif //CHIP_8_CPU_H

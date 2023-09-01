#ifndef CHIP_8_CPU_H
#define CHIP_8_CPU_H

#include <cstdint>
#include <utility>
#include "../memory/Memory.h"
#include "../display/Display.h"
#include "../keyboard/Keyboard.h"
#define FONT_MEMORY_LOC 0x50
#define BIG_FONT_MEMORY_LOC (FONT_MEMORY_LOC + 80)
#define BYTES_PER_DIGIT 5
#define BIG_BYTES_PER_DIGIT 10

class Cpu {
public:
    Cpu(Memory& mem, Display& dis, Keyboard& kb) :
        memory{mem}, display{dis}, keyboard{kb}, sp{&mem.stack} {};
    void cycle();
    void superChip();
    bool setQuirk(const std::string&);
    void frameUpdate();

    const std::uint16_t& pc {pc_};
    const std::uint16_t& cir {cir_};
    const std::uint16_t& i {i_};
    const std::array<std::uint8_t, 16>& reg {reg_};
    const std::uint8_t& delay {delay_};
    const std::uint8_t& sound {sound_};
    const bool& waitingForKeypress {wkp_};
private:
    // fetch decode+execute
    void fetch();
    void execute();
    // instructions
    void clearScreen();
    void ret();
    void jump(std::uint16_t);
    void call(uint16_t);
    void skipIfEqImm(std::uint8_t, std::uint8_t);
    void skipIfNeImm(std::uint8_t, std::uint8_t);
    void skipIfEq(std::uint8_t, std::uint8_t);
    void loadImm(std::uint8_t, std::uint8_t);
    void addImm(std::uint8_t, std::uint8_t);
    void loadReg(std::uint8_t, std::uint8_t);
    void orBits(std::uint8_t, std::uint8_t);
    void andBits(std::uint8_t, std::uint8_t);
    void xorBits(std::uint8_t, std::uint8_t);
    void add(std::uint8_t, std::uint8_t);
    void sub(std::uint8_t, std::uint8_t);
    void shr(std::uint8_t, std::uint8_t);
    void subn(std::uint8_t, std::uint8_t);
    void shl(std::uint8_t, std::uint8_t);
    void skipIfNe(std::uint8_t, std::uint8_t);
    void loadAddr(std::uint16_t);
    void jumpOffset(std::uint16_t, std::uint8_t);
    void rnd(std::uint8_t, std::uint8_t);
    void drw(std::uint8_t, std::uint8_t, std::uint8_t);
    void skipIfKeydown(std::uint8_t);
    void skipIfNotKeydown(std::uint8_t);
    void loadDelay(std::uint8_t);
    void waitForKeypress(std::uint8_t);
    void setDelay(std::uint8_t);
    void setSound(std::uint8_t);
    void setAddr(std::uint8_t);
    void loadFontLoc(std::uint8_t);
    void setBcd(std::uint8_t);
    void setRegs(std::uint8_t);
    void readIntoRegs(std::uint8_t);
    // superchip instructions
    void hires();
    void lores();
    void scrollDown(std::uint8_t);
    void scrollRight();
    void scrollLeft();
    void bigHex(std::uint8_t);
    void setFlags(std::uint8_t);
    void loadFlags(std::uint8_t);
    void exit();
    // registers
    std::uint16_t pc_ {0x200}; // program counter
    std::uint16_t cir_ {0}; // current instruction register
    std::uint16_t i_ {0}; // index register
    std::array<std::uint8_t, 16> reg_ {}; // general-purpose registers (V)
    std::array<std::uint8_t, 8> flag_ {}; // superchip flag registers
    std::uint8_t delay_ {0}; // delay timer register
    std::uint8_t sound_ {0}; // sound timer register
    bool wkp_ {false}; // waiting for key flag
    bool drawFlag {false}; // screen needs updated flag
    // quirks
    bool vfReset {true};
    bool incr {true};
    bool inplace {false};
    bool jumpx {false};
    bool overflow {false};
    // stack pointer
    std::stack<std::uint16_t>* sp;
    // hardware
    Memory& memory;
    Display& display;
    Keyboard& keyboard;
};


#endif //CHIP_8_CPU_H

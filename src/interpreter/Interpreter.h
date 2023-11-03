#ifndef CHIP_8_CPU_H
#define CHIP_8_CPU_H

#include <cstdint>
#include "../memory/Memory.h"
#include "../display/Display.h"
#include "../keyboard/Keyboard.h"

class Interpreter {
public:
    Interpreter(Memory& mem, Display& dis, Keyboard& kb) : memory{mem}, sp{&mem.stack}, display{dis}, keyboard{kb} {};

    void cycle();
    void endOfFrame();
    bool setMode(const std::string&);
    bool setQuirk(const std::string&);

    // references to internals for debugging
    const std::uint16_t& pc {pc_};
    const std::uint16_t& cir {cir_};
    const std::uint16_t& i {i_};
    const std::array<std::uint8_t, 16>& v {v_};
    const std::uint8_t& dt {dt_};
    const std::uint8_t& st {st_};
    const bool& waiting {waiting_};
private:
    static constexpr int vfReset {0};
    static constexpr int incr {1};
    static constexpr int inplace {2};
    static constexpr int jumpx {3};
    static constexpr int overflow {4};

    void fetch_();
    void execute_();

    [[nodiscard]] std::uint8_t op_() const { return cir_ >> 12 & 0xF; }
    [[nodiscard]] std::uint8_t x_() const { return cir_ >> 8 & 0xF; }
    [[nodiscard]] std::uint8_t y_() const { return cir_ >> 4 & 0xF; }
    [[nodiscard]] std::uint8_t n_() const { return cir_ & 0xF; }
    [[nodiscard]] std::uint8_t nn_() const { return cir_ & 0xFF; }
    [[nodiscard]] std::uint16_t nnn_() const { return cir_ & 0xFFF; }

    // instructions
    void cls_();
    void ret_();
    void jp_();
    void call_();
    void se_(std::uint8_t, std::uint8_t);
    void sne_(std::uint8_t, std::uint8_t);
    static void ld_(std::uint8_t&, std::uint8_t);
    void add_();
    void or_();
    void and_();
    void xor_();
    void adc_(std::uint8_t, std::uint8_t);
    void sub_(std::uint8_t, std::uint8_t);
    void shr_(std::uint8_t, std::uint8_t);
    void subn_(std::uint8_t, std::uint8_t);
    void shl_(std::uint8_t, std::uint8_t);
    void ldi_(std::uint16_t);
    void jpo_();
    void rnd_();
    void drw_();
    void skp_();
    void sknp_();
    void wkp_();
    void addi_(std::uint8_t);
    void bcd_();
    void sv_();
    void lv_();

    // superchip instructions
    void high_();
    void low_();
    void scd_();
    void scr_();
    void scl_();
    void exit_();
    void sf_();
    void lf_();

    std::uint16_t pc_ {0x200}; // program counter
    std::uint16_t cir_ {0}; // current instruction register
    std::uint16_t i_ {0}; // index register
    std::array<std::uint8_t, 16> v_ {}; // general purpose registers
    std::uint8_t dt_ {0}; // dt timer register
    std::uint8_t st_ {0}; // st timer register
    std::array<std::uint8_t, 8> flag_ {}; // superchip flag registers

    bool waiting_ {false}; // waiting for key flag
    bool draw_ {false}; // draw flag

    // defaults to chip-8 quirks
    std::array<bool, 5> quirk_ {true, true, false, false, false};

    // stack pointer
    std::stack<std::uint16_t>* sp;
    // hardware
    Memory& memory;
    Keyboard& keyboard;
    Display& display;
};


#endif //CHIP_8_CPU_H

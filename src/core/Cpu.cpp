#include "Cpu.h"
#include <iostream>
#include <iomanip>
#include <experimental/random>

void Cpu::cycle()
{
    fetch();
    execute();

    ++cycled;
    if (cycled == threshold) {
        updTimers();
        cycled = 0;
    }
}

void Cpu::updTimers()
{
    if (dt_ > 0)
        --dt_;

    if (st_ > 0) {
        // TODO: implement sound? (probably never)
        --st_;
    }
}

void Cpu::reset()
{
    pc_ = 0x200;
    cir_ = 0;
    i_ = 0;
    for (auto& byte : reg_)
        byte = 0;
    dt_ = 0;
    st_ = 0;

    while (!sp->empty())
        sp->pop();
}

void Cpu::fetch()
{
    std::uint16_t opcode {memory.read(pc_)};
    ++pc_;
    opcode <<= 8;
    opcode |= memory.read(pc_);
    ++pc_;
    cir_ = opcode;
}

void Cpu::execute() {
    const auto x {static_cast<std::uint8_t>(cir_ >> 8 & 0xF)};
    const auto y {static_cast<std::uint8_t>(cir_ >> 4 & 0xF)};
    const auto n {static_cast<std::uint8_t>(cir_ & 0xF)};
    const auto nn {static_cast<std::uint8_t>(cir_ & 0xFF)};
    const auto nnn {static_cast<std::uint16_t>(cir_ & 0xFFF)};

    switch (static_cast<std::uint8_t>(cir_ >> 12 & 0xF)) {
        case 0:
            switch (n) {
                case 0x0:
                    cls();
                    return;
                case 0xE:
                    ret();
                    return;
            }
        case 1:
            jp(nnn);
            return;
        case 2:
            call(nnn);
            return;
        case 3:
            sei(x, nn);
            return;
        case 4:
            snei(x, nn);
            return;
        case 5:
            se(x, y);
            return;
        case 6:
            ldi(x, nn);
            return;
        case 7:
            addi(x, nn);
            return;
        case 8:
            switch (n) {
                case 0:
                    ld(x, y);
                    return;
                case 1:
                    orr(x, y);
                    return;
                case 2:
                    andd(x, y);
                    return;
                case 3:
                    xorr(x, y);
                    return;
                case 4:
                    add(x, y);
                    return;
                case 5:
                    sub(x, y);
                    return;
                case 6:
                    shr(x, y);
                    return;
                case 7:
                    subn(x, y);
                    return;
                case 0xE:
                    shl(x, y);
                    return;
            }
        case 9:
            sne(x, y);
            return;
        case 0xA:
            ldaddr(nnn);
            return;
        case 0xB:
            jpr(nnn);
            return;
        case 0xC:
            rnd(x, nn);
            return;
        case 0xD:
            drw(x, y, n);
            return;
        case 0xE:
            switch (n) {
                case 0xE:
                    skp(x);
                    return;
                case 0x1:
                    sknp(x);
                    return;
            }
        case 0xF:
            switch (nn) {
                case 0x07:
                    lddt(x);
                    return;
                case 0x0A:
                    ldkp(x);
                    return;
                case 0x15:
                    sdt(x);
                    return;
                case 0x18:
                    sst(x);
                    return;
                case 0x1E:
                    saddr(x);
                    return;
                case 0x29:
                    spr(x);
                    return;
                case 0x33:
                    sbcd(x);
                    return;
                case 0x55:
                    sreg(x);
                    return;
                case 0x65:
                    rd(x);
                    return;
            }
        default:
            std::cerr << "[error] undefined opcode: " << std::hex << (int) cir << std::endl;
            break;
    }
}

// 0nnn: Jump to a machine code routine at nnn. *NOT IMPLEMENTED*

// 00E0: Clear the display.
inline void Cpu::cls()
{
    display.clear();
}

// 00EE: Return from a subroutine.
inline void Cpu::ret()
{
    pc_ = sp->top();
    sp->pop();
}

// 1nnn: Jump to location nnn.
inline void Cpu::jp(std::uint16_t nnn)
{
    pc_ = nnn;
}

// 2nnn: Call subroutine at nnn
inline void Cpu::call(std::uint16_t nnn)
{
    sp->push(pc);
    pc_ = nnn;
}

// 3xnn: Skip next instruction if Vx = nn.
inline void Cpu::sei(std::uint8_t x, std::uint8_t nn)
{
    if (reg_[x] == nn)
        pc_ += 2;
}

// 4xnn: Skip next instruction if Vx != nn.
inline void Cpu::snei(std::uint8_t x, std::uint8_t nn)
{
    if (reg_[x] != nn)
        pc_ += 2;
}

// 5xy0: Skip next instruction if Vx = Vy.
inline void Cpu::se(std::uint8_t x, std::uint8_t y)
{
    if (reg_[x] == reg_[y])
        pc_ += 2;
}

// 6xnn: Set Vx = nn.
inline void Cpu::ldi(std::uint8_t x, std::uint8_t nn)
{
    reg_[x] = nn;
}

// 7xnn: Set Vx = Vx + nn.
inline void Cpu::addi(std::uint8_t x, std::uint8_t nn)
{
    reg_[x] += nn;
}

// 8xy0: Set Vx = Vy.
inline void Cpu::ld(std::uint8_t x, std::uint8_t y)
{
    reg_[x] = reg_[y];
}

// 8xy1: Set Vx = Vx OR Vy.
inline void Cpu::orr(std::uint8_t x, std::uint8_t y)
{
    reg_[x] |= reg_[y];
}

// 8xy2: Set Vx = Vx AND Vy.
inline void Cpu::andd(std::uint8_t x, std::uint8_t y)
{
    reg_[x] &= reg_[y];
}

// 8xy3: Set Vx = Vx XOR Vy.
inline void Cpu::xorr(std::uint8_t x, std::uint8_t y)
{
    reg_[x] ^= reg_[y];
}

// 8xy4: Set Vx = Vx + Vy, set VF = carry.
inline void Cpu::add(std::uint8_t x, std::uint8_t y)
{
    auto flag {0xFF < reg_[x] + reg_[y]  ? 1 : 0};
    reg_[x] += reg_[y];
    reg_[0xF] = flag;
}

// 8xy5: Set Vx = Vx - Vy, set VF = NOT borrow.
inline void Cpu::sub(std::uint8_t x, std::uint8_t y)
{
    auto flag {reg_[x] > reg_[y] ? 1 : 0};
    reg_[x] -= reg_[y];
    reg_[0xF] = flag;
}

// 8xy6: Set Vx = Vx SHR 1.
inline void Cpu::shr(std::uint8_t x, std::uint8_t y)
{
    auto flag {(reg_[y] & 1) == 1 ? 1 : 0};
    reg_[x] = reg_[y] >> 1;
    reg_[0xF] = flag;
}

// 8xy7: Set Vx = Vy - Vx, set VF = NOT borrow.
inline void Cpu::subn(std::uint8_t x, std::uint8_t y)
{
    auto flag {reg_[y] > reg_[x] ? 1 : 0};
    reg_[x] = reg_[y] - reg_[x];
    reg_[0xF] = flag;
}

// 8xyE: Set Vx = Vx SHL 1.
inline void Cpu::shl(std::uint8_t x, std::uint8_t y)
{
    auto flag {(reg_[y] & 0x80) == 0x80 ? 1 : 0};
    reg_[x] = reg_[y] << 1;
    reg_[0xF] = flag;
}

// 9xy0: Skip next instruction if Vx != Vy.
void Cpu::sne(std::uint8_t x, std::uint8_t y)
{
    if (reg_[x] != reg_[y])
        pc_ += 2;
}

// Annn: Set I = nnn.
inline void Cpu::ldaddr(std::uint16_t nnn)
{
    i_ = nnn;
}

// Bnnn: Jump to location nnn + V0
inline void Cpu::jpr(std::uint16_t nnn)
{
    pc_ = nnn + reg_[0];
}

// Cxnn: Set Vx = random byte AND nn.
inline void Cpu::rnd(std::uint8_t x, std::uint8_t nn)
{
    reg_[x] = std::experimental::randint(0, 0xF) & nn;
}

// Dxyn: display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
inline void Cpu::drw(std::uint8_t x, std::uint8_t y, std::uint8_t n)
{
    reg_[0xF] = 0;
    auto xcoord {reg_[x] & (SCREEN_WIDTH-1)};
    auto ycoord {reg_[y] & (SCREEN_HEIGHT-1)};

    for (std::uint8_t h {0}; h != n && ycoord + h != SCREEN_HEIGHT; ++h) {
        auto byte {memory.read(i_ + h)};
        for (int right {0}; right != 8 &&   xcoord + right != SCREEN_WIDTH; ++right) {
            auto bit {byte << right & 0x80};
            bool collision = display.setPixel(xcoord + right, ycoord + h, bit);
            if (collision && !reg_[0xF])
                reg_[0xF] = 1;
        }
    }

    display.draw();
}

// Ex9E: Skip next instruction if key with the value of Vx is pressed.
inline void Cpu::skp(std::uint8_t x)
{
    if (keyboard.isPressed(reg_[x]))
        pc_ += 2;
}

// ExA1: Skip next instruction if key with the value of Vx is not pressed.
inline void Cpu::sknp(std::uint8_t x)
{
    if (!keyboard.isPressed(reg_[x]))
        pc_ += 2;
}

// Fx07: Set Vx = delay timer value.
inline void Cpu::lddt(std::uint8_t x)
{
    reg_[x] = dt_;
}

// Fx0A: Wait for a key press, store the value of the key in Vx.
inline void Cpu::ldkp(std::uint8_t x)
{
    if (!wkp_)
        wkp_ = true;
    std::uint8_t key {keyboard.wasPressed()};
    if (key == NULL_KEY)
        pc_ -= 2; // loop until key is pressed
    else {
        reg_[x] = key;
        wkp_ = false;
    }
}

// Fx15: Set delay timer = Vx.
inline void Cpu::sdt(std::uint8_t x)
{
    dt_ = reg_[x];
}

// Fx18: Set sound timer = Vx.
inline void Cpu::sst(std::uint8_t x)
{
    st_ = reg_[x];
}

// Fx1E: Set I = I + Vx.
inline void Cpu::saddr(std::uint8_t x)
{
    auto flag {999 < i_ + reg_[x] ? 1 : 0};
    i_ += reg_[x];
    reg_[0xF] = flag;
}

// Fx29: Set I = location of sprite for digit Vx.
inline void Cpu::spr(std::uint8_t x)
{
    i_ = FONT_MEMORY_LOC + (reg_[x] * BYTES_PER_DIGIT);
}

// Fx33: Store BCD representation of Vx in memory locations I, I+1, and I+2.
inline void Cpu::sbcd(std::uint8_t x)
{
    auto byte {reg_[x]};
    memory.write(byte / 100, i_);
    memory.write(byte / 10 % 10, i_ + 1);
    memory.write(byte % 10, i_ + 2);
}

// Fx55: Store registers V0 through Vx in memory starting at location I.
inline void Cpu::sreg(std::uint8_t x)
{
    for (int r {0}; r != x + 1; ++r)
        memory.write(reg_[r], i_ + r);
}

// Fx65: Read registers V0 through Vx from memory starting at location I.
inline void Cpu::rd(std::uint8_t x)
{
    for (int r {0}; r != x + 1; ++r)
        reg_[r] = memory.read(i_ + r);
}
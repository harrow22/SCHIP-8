#include "Cpu.h"
#include <iostream>
#include <experimental/random>

void onUndefinedOpcode(std::uint8_t cir)
{
    std::cerr << "[error] undefined opcode: " << std::hex << (int) cir << std::endl;
}

void Cpu::superChip() {
    vfReset = false;
    incr = false;
    inplace = true;
    jumpx = true;
}

bool Cpu::setQuirk(const std::string& quirk) {
    std::string delimiter = "=";
    std::string name = quirk.substr(0, quirk.find(delimiter));
    std::string enabled = quirk.substr(quirk.find(delimiter), quirk.size());

    if (name == "vf_reset")
        vfReset = enabled == "true";
    else if (name == "memory")
        incr = enabled == "true";
    else if (name == "shifting")
        inplace = enabled == "true";
    else if (name == "jumping")
        jumpx = enabled == "true";
    else if (name == "overflow")
        overflow = enabled == "true";
    else
        return false;

    return true;
}

void Cpu::cycle()
{
    fetch();
    execute();
}

void Cpu::frameUpdate()
{
    // decrement timers
    if (delay_ > 0)
        --delay_;

    if (sound_ > 0) {
        // TODO: implement sound (probably will never do this)
        --sound_;
    }

    // render any changes
    if (drawFlag) {
        display.draw();
        drawFlag = false;
    }
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
            switch (nn) {
                case 0xE0:
                    clearScreen();
                    return;
                case 0xEE:
                    ret();
                    return;
                case 0xFF:
                    hires();
                    return;
                case 0xFE:
                    lores();
                    return;
                case 0xFB:
                    scrollRight();
                    return;
                case 0xFC:
                    scrollLeft();
                    return;
                case 0xFD:
                    exit();
                    return;
                default:
                    if (y == 0xC)
                        scrollDown(n);
                    else
                        onUndefinedOpcode(cir_);
                    return;
            }
        case 1:
            jump(nnn);
            return;
        case 2:
            call(nnn);
            return;
        case 3:
            skipIfEqImm(x, nn);
            return;
        case 4:
            skipIfNeImm(x, nn);
            return;
        case 5:
            skipIfEq(x, y);
            return;
        case 6:
            loadImm(x, nn);
            return;
        case 7:
            addImm(x, nn);
            return;
        case 8:
            switch (n) {
                case 0:
                    loadReg(x, y);
                    return;
                case 1:
                    orBits(x, y);
                    return;
                case 2:
                    andBits(x, y);
                    return;
                case 3:
                    xorBits(x, y);
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
                default:
                    return onUndefinedOpcode(cir_);
            }
        case 9:
            skipIfNe(x, y);
            return;
        case 0xA:
            loadAddr(nnn);
            return;
        case 0xB:
            jumpOffset(nnn, x);
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
                    skipIfKeydown(x);
                    return;
                case 0x1:
                    skipIfNotKeydown(x);
                    return;
                default:
                    return onUndefinedOpcode(cir_);
            }
        case 0xF:
            switch (nn) {
                case 0x07:
                    loadDelay(x);
                    return;
                case 0x0A:
                    waitForKeypress(x);
                    return;
                case 0x15:
                    setDelay(x);
                    return;
                case 0x18:
                    setSound(x);
                    return;
                case 0x1E:
                    setAddr(x);
                    return;
                case 0x29:
                    loadFontLoc(x);
                    return;
                case 0x33:
                    setBcd(x);
                    return;
                case 0x55:
                    setRegs(x);
                    return;
                case 0x65:
                    readIntoRegs(x);
                    return;
                case 0x30:
                    bigHex(x);
                    return;
                case 0x75:
                    setFlags(x);
                    return;
                case 0x85:
                    loadFlags(x);
                    return;
                default:
                    return onUndefinedOpcode(cir_);
            }
        default:
            return onUndefinedOpcode(cir_);
    }
}

// 0nnn: Jump to a machine code routine at nnn. *NOT IMPLEMENTED*

// 00E0: Clear the display.
inline void Cpu::clearScreen()
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
inline void Cpu::jump(std::uint16_t nnn)
{
    pc_ = nnn;
}

// 2nnn: Call subroutine at nnn
inline void Cpu::call(std::uint16_t nnn)
{
    sp->push(pc_);
    pc_ = nnn;
}

// 3xnn: Skip next instruction if Vx = nn.
inline void Cpu::skipIfEqImm(std::uint8_t x, std::uint8_t nn)
{
    if (reg_[x] == nn)
        pc_ += 2;
}

// 4xnn: Skip next instruction if Vx != nn.
inline void Cpu::skipIfNeImm(std::uint8_t x, std::uint8_t nn)
{
    if (reg_[x] != nn)
        pc_ += 2;
}

// 5xy0: Skip next instruction if Vx = Vy.
inline void Cpu::skipIfEq(std::uint8_t x, std::uint8_t y)
{
    if (reg_[x] == reg_[y])
        pc_ += 2;
}

// 6xnn: Set Vx = nn.
inline void Cpu::loadImm(std::uint8_t x, std::uint8_t nn)
{
    reg_[x] = nn;
}

// 7xnn: Set Vx = Vx + nn.
inline void Cpu::addImm(std::uint8_t x, std::uint8_t nn)
{
    reg_[x] += nn;
}

// 8xy0: Set Vx = Vy.
inline void Cpu::loadReg(std::uint8_t x, std::uint8_t y)
{
    reg_[x] = reg_[y];
}

// 8xy1: Set Vx = Vx OR Vy.
inline void Cpu::orBits(std::uint8_t x, std::uint8_t y)
{
    reg_[x] |= reg_[y];
    if (vfReset)
        reg_[0xF] = 0;
}

// 8xy2: Set Vx = Vx AND Vy.
inline void Cpu::andBits(std::uint8_t x, std::uint8_t y)
{
    reg_[x] &= reg_[y];
    if (vfReset)
        reg_[0xF] = 0;
}

// 8xy3: Set Vx = Vx XOR Vy.
inline void Cpu::xorBits(std::uint8_t x, std::uint8_t y)
{
    reg_[x] ^= reg_[y];
    if (vfReset)
        reg_[0xF] = 0;
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
    auto flag {reg_[x] >= reg_[y] ? 1 : 0};
    reg_[x] -= reg_[y];
    reg_[0xF] = flag;
}

// 8xy6: Set Vx = Vx SHR 1.
inline void Cpu::shr(std::uint8_t x, std::uint8_t y)
{
    auto flag {(reg_[y] & 1) == 1 ? 1 : 0};
    reg_[x] = (inplace ? reg_[x] : reg_[y]) >> 1;
    reg_[0xF] = flag;
}

// 8xy7: Set Vx = Vy - Vx, set VF = NOT borrow.
inline void Cpu::subn(std::uint8_t x, std::uint8_t y)
{
    auto flag {reg_[y] >= reg_[x] ? 1 : 0};
    reg_[x] = reg_[y] - reg_[x];
    reg_[0xF] = flag;
}

// 8xyE: Set Vx = Vx SHL 1.
inline void Cpu::shl(std::uint8_t x, std::uint8_t y)
{
    auto flag {(reg_[y] & 0x80) == 0x80 ? 1 : 0};
    reg_[x] = (inplace ? reg_[x] : reg_[y]) << 1;
    reg_[0xF] = flag;
}

// 9xy0: Skip next instruction if Vx != Vy.
void Cpu::skipIfNe(std::uint8_t x, std::uint8_t y)
{
    if (reg_[x] != reg_[y])
        pc_ += 2;
}

// Annn: Set I = nnn.
inline void Cpu::loadAddr(std::uint16_t nnn)
{
    i_ = nnn;
}

// Bnnn: Jump to location nnn + V0
inline void Cpu::jumpOffset(std::uint16_t nnn, std::uint8_t x)
{
    pc_ = nnn + (jumpx ? reg_[x] : reg_[0]);
}

// Cxnn: Set Vx = random byte AND nn.
inline void Cpu::rnd(std::uint8_t x, std::uint8_t nn)
{
    reg_[x] = std::experimental::randint(0, 0xFF) & nn;
}

// Dxyn: display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
// Dxy0: Draw a 16x16 sprite. If used by CHIP-8 program, will still function like SuperChip.
inline void Cpu::drw(std::uint8_t x, std::uint8_t y, std::uint8_t n)
{
    int bitmax {n == 0 ? 16 : 8};
    int scale {n == 0 ? 2 : 1};
    int ylim {n == 0 ? 16 * scale : n};

    int xcoord {reg_[x] & (display.width - 1)};
    int ycoord {reg_[y] & (display.height - 1)};

    reg_[0xF] = 0;
    for (int yline {0}; yline != ylim && ycoord + yline/scale != display.height; yline += scale) {
        int byte {bitmax == 16
                  ? memory.read(i_ + yline) << 8 | memory.read(i_ + yline + 1)
                  : memory.read(i_ + yline)};

        // loop through the bits
        for (int xline {0}; xline != bitmax && xcoord + xline != display.width; ++xline) {
            if ((byte << xline & (bitmax == 16 ? 0x8000 : 0x80)) != 0) {
                bool collision {display.flipPixel(xcoord + xline, ycoord + yline/scale)};
                if (collision)
                    reg_[0xF] = 1;
            }
        }
    }
    drawFlag = true;
}

// Ex9E: Skip next instruction if key with the value of Vx is pressed.
inline void Cpu::skipIfKeydown(std::uint8_t x)
{
    if (Keyboard::isPressed(reg_[x]))
        pc_ += 2;
}

// ExA1: Skip next instruction if key with the value of Vx is not pressed.
inline void Cpu::skipIfNotKeydown(std::uint8_t x)
{
    if (!Keyboard::isPressed(reg_[x]))
        pc_ += 2;
}

// Fx07: Set Vx = delay timer value.
inline void Cpu::loadDelay(std::uint8_t x)
{
    reg_[x] = delay_;
}

// Fx0A: Wait for a key press, store the value of the key in Vx.
inline void Cpu::waitForKeypress(std::uint8_t x)
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
inline void Cpu::setDelay(std::uint8_t x)
{
    delay_ = reg_[x];
}

// Fx18: Set sound timer = Vx.
inline void Cpu::setSound(std::uint8_t x)
{
    sound_ = reg_[x];
}

// Fx1E: Set I = I + Vx.
inline void Cpu::setAddr(std::uint8_t x)
{
    if (overflow && i + reg_[x] > 999)
        reg_[0xF] = 0;
    i_ += reg_[x];
}

// Fx29: Set I = location of sprite for digit Vx.
inline void Cpu::loadFontLoc(std::uint8_t x)
{
    i_ = FONT_MEMORY_LOC + (reg_[x] * BYTES_PER_DIGIT);
}

// Fx33: Store BCD representation of Vx in memory locations I, I+1, and I+2.
inline void Cpu::setBcd(std::uint8_t x)
{
    auto byte {reg_[x]};
    memory.write(byte / 100, i_);
    memory.write(byte / 10 % 10, i_ + 1);
    memory.write(byte % 10, i_ + 2);
}

// Fx55: Store registers V0 through Vx in memory starting at location I.
inline void Cpu::setRegs(std::uint8_t x)
{
    for (int r {0}; r != x + 1; ++r) {
        if (incr)
            memory.write(reg_[r], i_++);
        else
            memory.write(reg_[r], i_ + r);
    }

}

// Fx65: Read registers V0 through Vx from memory starting at location I.
inline void Cpu::readIntoRegs(std::uint8_t x)
{
    for (int r {0}; r != x + 1; ++r) {
        if (incr)
            reg_[r] = memory.read(i_++);
        else
            reg_[r] = memory.read(i_ + r);
    }
}

// SuperChip instructions start here
// 00FF: Enable 128x64 high resolution graphics mode.
inline void Cpu::hires()
{
    display.setres(HI_RESOLUTION_SCALE);
}

// 00FE: Disable high resolution graphics mode and return to 64x32.
inline void Cpu::lores()
{
    display.setres(1);
}

// 00Cn: Scroll the display down by n [0, 15] pixels.
inline void Cpu::scrollDown(std::uint8_t n)
{
    display.scrollDown(n);
    drawFlag = true;
}

// 00FB: Scroll the display right by 4 pixels.
inline void Cpu::scrollRight()
{
    display.scrollRight();
    drawFlag = true;
}

// 00FC: Scroll the display left by 4 pixels.
inline void Cpu::scrollLeft()
{
    display.scrollLeft();
    drawFlag = true;
}

// Fx30: Set i to a large hexadecimal character based on the value of Vx.
inline void Cpu::bigHex(std::uint8_t x)
{
    i_ = BIG_FONT_MEMORY_LOC + (reg_[x] * BIG_BYTES_PER_DIGIT);
}

// Fx75: Save V0 - Vx to flag registers.
inline void Cpu::setFlags(std::uint8_t x)
{
    for (int r {0}; r != x + 1 && r != 8; ++r)
        flag_[r] = reg_[r];

}

// Fx85: Restore V0 - Vx from flag registers.
inline void Cpu::loadFlags(std::uint8_t x)
{
    for (int r {0}; r != x + 1 && r != 8; ++r)
        reg_[r] = flag_[r];
}

// 00FD: Exit the Chip8/SuperChip interpreter. *PROGRAM WILL LOOP INDEFINITELY*
inline void Cpu::exit()
{
    pc_ -= 2;
}
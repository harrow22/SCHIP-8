#include "Interpreter.h"
#include <iostream>
#include <string>
#include <format>
#include <experimental/random>

bool Interpreter::setMode(const std::string& mode) {
    if (mode == "superchip") {
        quirk_ = {false, false, true, true, quirk_[ioverflow]};;
    } else if (mode == "xochip") {
        quirk_ = {false, true, false, false, quirk_[ioverflow]};;
    } else if (mode == "default") {
        quirk_ = {true, true, false, false, quirk_[ioverflow]};
    } else {
        return false;
    }
    return true;
}

bool Interpreter::setQuirk(const std::string& quirk) {
    std::string delimiter = "=";
    std::string name = quirk.substr(0, quirk.find(delimiter));
    std::string enabled = quirk.substr(quirk.find(delimiter), quirk.size());

    if (name == "vf_reset")
        quirk_[vfReset] = enabled == "true";
    else if (name == "memory")
        quirk_[incr] = enabled == "true";
    else if (name == "shifting")
        quirk_[inplace] = enabled == "true";
    else if (name == "jumping")
        quirk_[jumpx] = enabled == "true";
    else if (name == "ioverflow")
        quirk_[ioverflow] = enabled == "true";
    else
        return false;

    return true;
}

void Interpreter::cycle()
{
    fetch_();
    execute_();
}

void Interpreter::endOfFrame()
{
    // decrement timers
    if (dt_ > 0)
        --dt_;

    if (st_ > 0) {
        // TODO: implement st (probably will never do this)
        --st_;
    }

    // render any changes
    if (draw_) {
        display.draw();
        draw_ = false;
    }
}

void Interpreter::fetch_()
{
    cir_ = memory.read(pc_++);
    cir_ <<= 8;
    cir_ |= memory.read(pc_++);
}

void undefined(std::uint8_t cir)
{
    std::cerr << std::format("[error] undefined opcode: {:0>4X}\n", cir);
}

void Interpreter::execute_() {
    switch (op_()) {
        case 0:
            switch (nn_()) {
                case 0xE0: return cls_();
                case 0xEE: return ret_();
                case 0xFF: return high_();
                case 0xFE: return low_();
                case 0xFB: return scr_();
                case 0xFC: return scl_();
                case 0xFD: return exit_();
                default:
                    if (y_() == 0xC) return scd_();
                    else return undefined(cir_);
            }
        case 1: return jp_();
        case 2: return call_();
        case 3: return se_(v_[x_()], nn_());
        case 4: return sne_(v_[x_()], nn_());
        case 5: return se_(v_[x_()], v_[y_()]);
        case 6: return ld_(v_[x_()], nn_());
        case 7: return add_();
        case 8:
            switch (n_()) {
                case 0: return ld_(v_[x_()], v_[y_()]);
                case 1: return or_();
                case 2: return and_();
                case 3: return xor_();
                case 4: return adc_(x_(), y_());
                case 5: return sub_(x_(), y_());
                case 6: return shr_(x_(), y_());
                case 7: return subn_(x_(), y_());
                case 0xE: return shl_(x_(), y_());
                default: return undefined(cir_);
            }
        case 9: return sne_(v_[x_()], v_[y_()]);
        case 0xA: return ldi_(nnn_());
        case 0xB: return jpo_();
        case 0xC: return rnd_();
        case 0xD: return drw_();
        case 0xE:
            switch (n_()) {
                case 0xE: return skp_();
                case 0x1: return sknp_();
                default: return undefined(cir_);
            }
        case 0xF:
            switch (nn_()) {
                case 0x07: return ld_(v_[x_()], dt_);
                case 0x0A: return wkp_();
                case 0x15: return ld_(dt_, v_[x_()]);
                case 0x18: return ld_(st_, v_[x_()]);
                case 0x1E: return addi_(x_());
                case 0x29: return ldi_(memory.getFont(v_[x_()]));
                case 0x33: return bcd_();
                case 0x55: return sv_();
                case 0x65: return lv_();
                case 0x30: return ldi_(memory.getBigFont(v_[x_()]));
                case 0x75: return sf_();
                case 0x85: return lf_();
                default: return undefined(cir_);
            }
        default: return undefined(cir_);
    }
}

// 0nnn: Jump to a machine code routine at nnn. *NOT IMPLEMENTED*

// 00E0: Clear the display.
inline void Interpreter::cls_()
{
    display.clear();
}

// 00EE: Return from a subroutine.
inline void Interpreter::ret_()
{
    pc_ = sp->top();
    sp->pop();
}

// 1nnn: Jump to location nnn.
inline void Interpreter::jp_()
{
    pc_ = nnn_();
}

// 2nnn: Call subroutine at nnn
inline void Interpreter::call_()
{
    sp->push(pc_);
    pc_ = nnn_();
}

// 3xnn: Skip next instruction if Vx = nn.
// 5xy0: Skip next instruction if Vx = Vy.
inline void Interpreter::se_(std::uint8_t a, std::uint8_t b)
{
    if (a == b)
        pc_ += 2;
}

// 4xnn: Skip next instruction if Vx != nn.
// 9xy0: Skip next instruction if Vx != Vy.
inline void Interpreter::sne_(std::uint8_t a, std::uint8_t b)
{
    if (a != b)
        pc_ += 2;
}

// 6xnn: Set Vx = nn.
// 8xy0: Set Vx = Vy.
// Fx07: Set Vx = dt timer value.
// Fx15: Set dt timer = Vx.
// Fx18: Set st timer = Vx.
inline void Interpreter::ld_(std::uint8_t& dst, std::uint8_t val)
{
    dst = val;
}

// 7xnn: Set Vx = Vx + nn.
inline void Interpreter::add_()
{
    v_[x_()] += nn_();
}


// 8xy1: Set Vx = Vx OR Vy.
inline void Interpreter::or_()
{
    v_[x_()] |= v_[y_()];
    if (quirk_[vfReset])
        v_[0xF] = 0;
}

// 8xy2: Set Vx = Vx AND Vy.
inline void Interpreter::and_()
{
    v_[x_()] &= v_[y_()];
    if (quirk_[vfReset])
        v_[0xF] = 0;
}

// 8xy3: Set Vx = Vx XOR Vy.
inline void Interpreter::xor_()
{
    v_[x_()] ^= v_[y_()];
    if (quirk_[vfReset])
        v_[0xF] = 0;
}

// 8xy4: Set Vx = Vx + Vy, set VF = carry.
inline void Interpreter::adc_(std::uint8_t x, std::uint8_t y)
{
    int vf {v_[x] + v_[y] > 0xFF ? 1 : 0};
    v_[x] += v_[y];
    v_[0xF] = vf;
}

// 8xy5: Set Vx = Vx - Vy, set VF = NOT borrow.
inline void Interpreter::sub_(std::uint8_t x, std::uint8_t y)
{
    int vf {v_[y] < v_[x] ? 1 : 0};
    v_[x] = v_[x] - v_[y];
    v_[0xF] = vf;
}

// 8xy6: Set Vx = Vx SHR 1.
inline void Interpreter::shr_(std::uint8_t x, std::uint8_t y)
{
    int vf {(v_[y] & 1) == 1 ? 1 : 0};
    v_[x] = (quirk_[inplace] ? v_[x] : v_[y]) >> 1;
    v_[0xF] = vf;
}

// 8xy7: Set Vx = Vy - Vx, set VF = NOT borrow.
inline void Interpreter::subn_(std::uint8_t x, std::uint8_t y)
{
    int vf {v_[x] < v_[y] ? 1 : 0};
    v_[x] = v_[y] - v_[x];
    v_[0xF] = vf;
}

// 8xyE: Set Vx = Vx SHL 1.
inline void Interpreter::shl_(std::uint8_t x, std::uint8_t y)
{
    int vf {(v_[y] & 0x80) == 0x80 ? 1 : 0};
    v_[x] = (quirk_[inplace] ? v_[x] : v_[y]) << 1;
    v_[0xF] = vf;
}

// Annn: Set I = nnn.
// Fx29: Set I = location of sprite for digit Vx.
// Fx30: Set i to a large hexadecimal character based on the value of Vx.
inline void Interpreter::ldi_(std::uint16_t val)
{
    i_ = val;
}

// Bnnn: Jump to location nnn + V0
inline void Interpreter::jpo_()
{
    pc_ = nnn_() + (quirk_[jumpx] ? v_[x_()] : v_[0]);
}

// Cxnn: Set Vx = random byte AND nn.
inline void Interpreter::rnd_()
{
    v_[x_()] = std::experimental::randint(0, 0xFF) & nn_();
}

// Dxyn: display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
// Dxy0: Draw a 16x16 sprite. If used by CHIP-8 program, will still function like SuperChip.
inline void Interpreter::drw_()
{
    std::uint8_t n {n_()};
    int bitmax {n == 0 ? 16 : 8};
    int scale {n == 0 ? 2 : 1};
    int ylim {n == 0 ? 16 * scale : n};

    int xcoord {v_[x_()] & (display.width - 1)};
    int ycoord {v_[y_()] & (display.height - 1)};

    v_[0xF] = 0;
    for (int yline {0}; yline != ylim and ycoord + yline/scale != display.height; yline += scale) {
        int byte {bitmax == 16
                  ? memory.read(i_ + yline) << 8 | memory.read(i_ + yline + 1)
                  : memory.read(i_ + yline)};

        // loop through the bits
        for (int xline {0}; xline != bitmax and xcoord + xline != display.width; ++xline) {
            if ((byte << xline & (bitmax == 16 ? 0x8000 : 0x80)) != 0) {
                bool collision {display.flipPixel(xcoord + xline, ycoord + yline/scale)};
                if (collision)
                    v_[0xF] = 1;
            }
        }
    }
    draw_ = true;
}

// Ex9E: Skip next instruction if key with the value of Vx is pressed.
inline void Interpreter::skp_()
{
    if (Keyboard::isPressed(v_[x_()]))
        pc_ += 2;
}

// ExA1: Skip next instruction if key with the value of Vx is not pressed.
inline void Interpreter::sknp_()
{
    if (!Keyboard::isPressed(v_[x_()]))
        pc_ += 2;
}

// Fx0A: Wait for a key press, store the value of the key in Vx.
inline void Interpreter::wkp_()
{
    if (!waiting_)
        waiting_ = true;
    std::uint8_t key {keyboard.wasPressed()};
    if (key == Keyboard::nullKey)
        pc_ -= 2; // loop until key is pressed
    else {
        v_[x_()] = key;
        waiting_ = false;
    }
}

// Fx1E: Set I = I + Vx.
inline void Interpreter::addi_(std::uint8_t x)
{
    if (quirk_[ioverflow] and i + v_[x] > 999)
        v_[0xF] = 0;
    i_ += v_[x];
}

// Fx33: Store BCD representation of Vx in memory locations I, I+1, and I+2.
inline void Interpreter::bcd_()
{
    std::uint8_t byte {v_[x_()]};
    memory.write(byte / 100, i_);
    memory.write(byte / 10 % 10, i_ + 1);
    memory.write(byte % 10, i_ + 2);
}

// Fx55: Store registers V0 through Vx in memory starting at location I.
inline void Interpreter::sv_()
{
    std::uint8_t n {static_cast<uint8_t>(x_() + 1)};
    for (int r {0}; r != n; ++r) {
        if (quirk_[incr])
            memory.write(v_[r], i_++);
        else
            memory.write(v_[r], i_ + r);
    }
}

// Fx65: Read registers V0 through Vx from memory starting at location I.
inline void Interpreter::lv_()
{
    std::uint8_t n {static_cast<uint8_t>(x_() + 1)};
    for (int r {0}; r != n; ++r) {
        if (quirk_[incr])
            v_[r] = memory.read(i_++);
        else
            v_[r] = memory.read(i_ + r);
    }
}

// 00FF: Enable 128x64 high resolution graphics mode.
inline void Interpreter::high_()
{
    display.setResolution(2);
}

// 00FE: Disable high resolution graphics mode and return to 64x32.
inline void Interpreter::low_()
{
    display.setResolution(1);
}

// 00Cn: Scroll the display down by n [0, 15] pixels.
inline void Interpreter::scd_()
{
    display.scrollDown(n_());
    draw_ = true;
}

// 00FB: Scroll the display right by 4 pixels.
inline void Interpreter::scr_()
{
    display.scrollRight();
    draw_ = true;
}

// 00FC: Scroll the display left by 4 pixels.
inline void Interpreter::scl_()
{
    display.scrollLeft();
    draw_ = true;
}

// 00FD: Exit the Chip8/SuperChip interpreter. *PROGRAM WILL LOOP INDEFINITELY*
inline void Interpreter::exit_()
{
    pc_ -= 2;
}

// Fx75: Save V0 - Vx to flag registers.
inline void Interpreter::sf_()
{
    std::uint8_t n {static_cast<uint8_t>(v_[x_()] + 1)};
    for (int r {0}; r != n and r != 8; ++r)
        flag_[r] = v_[r];

}

// Fx85: Restore V0 - Vx from flag registers.
inline void Interpreter::lf_()
{
    std::uint8_t n {static_cast<uint8_t>(v_[x_()] + 1)};
    for (int r {0}; r != n and r != 8; ++r)
        v_[r] = flag_[r];
}
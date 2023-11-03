#include <gtest/gtest.h>
#include "Interpreter.h"

// Test suite for all opcodes minus input & display
class InterpreterTest : public testing::Test {
protected:
    void setRegisterInstr(std::uint8_t hi, std::uint8_t lo, std::uint16_t offset=0) {
        memory.write(hi, interpreter.pc + offset);
        memory.write(lo, interpreter.pc + offset + 1);
    }

    Memory memory {};
    Display display {};
    Keyboard keyboard {};
    Interpreter interpreter {memory, display, keyboard};
};

TEST_F(InterpreterTest, initialStateIsValid)
{
EXPECT_EQ(interpreter.pc, 0x200); // CHIP-8 programs start at memory addr 0x200
EXPECT_EQ(interpreter.cir, 0);
EXPECT_EQ(interpreter.i, 0);
for (int i = 0; i != 16; ++i)
EXPECT_EQ(interpreter.v[i], 0);
EXPECT_EQ(interpreter.st, 0);
EXPECT_EQ(interpreter.dt, 0);
}

TEST_F(InterpreterTest, instructionCallAndRet)
{
setRegisterInstr(0x24, 0x44);
setRegisterInstr(0x00, 0xEE, 0x444 - 0x200);
interpreter.cycle();
EXPECT_EQ(memory.stack.top(), 0x202);
interpreter.cycle();

EXPECT_EQ(interpreter.pc, 0x202);
EXPECT_EQ(interpreter.cir, 0x00EE);
}

TEST_F(InterpreterTest, instruction1nnn)
{
setRegisterInstr(0x20, 0x44);
interpreter.cycle();

EXPECT_EQ(interpreter.pc, 0x044);
EXPECT_EQ(interpreter.cir, 0x2044);
}

TEST_F(InterpreterTest, instruction3xnnSkip)
{
setRegisterInstr(0x60, 0x44);
interpreter.cycle();
setRegisterInstr(0x30, 0x44);
interpreter.cycle();

EXPECT_EQ(interpreter.pc, 0x206);
EXPECT_EQ(interpreter.cir, 0x3044);
}

TEST_F(InterpreterTest, instruction3xnnNoSkip)
{
setRegisterInstr(0x60, 0x44);
interpreter.cycle();
setRegisterInstr(0x30, 0x42);
interpreter.cycle();

EXPECT_EQ(interpreter.pc, 0x204);
EXPECT_EQ(interpreter.cir, 0x3042);
}

TEST_F(InterpreterTest, instruction4xnnSkip)
{
setRegisterInstr(0x60, 0x40);
interpreter.cycle();
setRegisterInstr(0x40, 0x44);
interpreter.cycle();

EXPECT_EQ(interpreter.pc, 0x206);
EXPECT_EQ(interpreter.cir, 0x4044);
}

TEST_F(InterpreterTest, instruction4xnnNoSkip)
{
setRegisterInstr(0x60, 0x44);
interpreter.cycle();
setRegisterInstr(0x40, 0x44);
interpreter.cycle();

EXPECT_EQ(interpreter.pc, 0x204);
EXPECT_EQ(interpreter.cir, 0x4044);
}

TEST_F(InterpreterTest, instruction5xy0Skip)
{
setRegisterInstr(0x60, 0x44);
interpreter.cycle();
setRegisterInstr(0x61, 0x44);
interpreter.cycle();
setRegisterInstr(0x50, 0x10);
interpreter.cycle();

EXPECT_EQ(interpreter.pc, 0x208);
EXPECT_EQ(interpreter.cir, 0x5010);
}

TEST_F(InterpreterTest, instruction5xy0NoSkip)
{
setRegisterInstr(0x60, 0x44);
interpreter.cycle();
setRegisterInstr(0x61, 0x42);
interpreter.cycle();
setRegisterInstr(0x50, 0x10);
interpreter.cycle();

EXPECT_EQ(interpreter.pc, 0x206);
EXPECT_EQ(interpreter.cir, 0x5010);
}

TEST_F(InterpreterTest, instruction6xnn)
{
setRegisterInstr(0x60, 0x12);
interpreter.cycle();

EXPECT_EQ(interpreter.pc, 0x202);
EXPECT_EQ(interpreter.cir, 0x6012);
EXPECT_EQ(interpreter.v[0], 0x12);
}

TEST_F(InterpreterTest, instruction7xnn)
{
setRegisterInstr(0x62, 0x12);
interpreter.cycle();

setRegisterInstr(0x72, 0x32);
interpreter.cycle();

EXPECT_EQ(interpreter.pc, 0x204);
EXPECT_EQ(interpreter.cir, 0x7232);
EXPECT_EQ(interpreter.v[2], 0x44);
}

TEST_F(InterpreterTest, instruction8xy0)
{
setRegisterInstr(0x60, 0x01);
interpreter.cycle();
setRegisterInstr(0x61, 0x02);
interpreter.cycle();
setRegisterInstr(0x82, 0x10);
interpreter.cycle();

EXPECT_EQ(interpreter.v[2], 2);
EXPECT_EQ(interpreter.cir, 0x8210);
}

TEST_F(InterpreterTest, instruction8xy1)
{
setRegisterInstr(0x60, 0x01);
interpreter.cycle();
setRegisterInstr(0x61, 0x02);
interpreter.cycle();
setRegisterInstr(0x80, 0x11);
interpreter.cycle();

EXPECT_EQ(interpreter.v[0], 3);
EXPECT_EQ(interpreter.cir, 0x8011);
}

TEST_F(InterpreterTest, instruction8xy2)
{
setRegisterInstr(0x60, 0x01);
interpreter.cycle();
setRegisterInstr(0x61, 0x02);
interpreter.cycle();
setRegisterInstr(0x80, 0x12);
interpreter.cycle();

EXPECT_EQ(interpreter.v[0], 0);
EXPECT_EQ(interpreter.cir, 0x8012);
}

TEST_F(InterpreterTest, instruction8xy3)
{
setRegisterInstr(0x60, 0x01);
interpreter.cycle();
setRegisterInstr(0x61, 0x02);
interpreter.cycle();
setRegisterInstr(0x80, 0x13);
interpreter.cycle();

EXPECT_EQ(interpreter.v[0], 3);
EXPECT_EQ(interpreter.cir, 0x8013);
}

TEST_F(InterpreterTest, instruction8xy4NoVf)
{
setRegisterInstr(0x60, 0xB);
interpreter.cycle();
setRegisterInstr(0x61, 0xA);
interpreter.cycle();
setRegisterInstr(0x80, 0x14);
interpreter.cycle();

EXPECT_EQ(interpreter.v[0], 0x15);
EXPECT_EQ(interpreter.v[0xF], 0);
EXPECT_EQ(interpreter.cir, 0x8014);
}

TEST_F(InterpreterTest, instruction8xy4Vf)
{
setRegisterInstr(0x60, 0x7F);
interpreter.cycle();
setRegisterInstr(0x61, 0x85);
interpreter.cycle();
setRegisterInstr(0x80, 0x14);
interpreter.cycle();

EXPECT_EQ(interpreter.v[0], 4);
EXPECT_EQ(interpreter.v[0xF], 1);
EXPECT_EQ(interpreter.cir, 0x8014);
}

TEST_F(InterpreterTest, instruction8xy5NoVf)
{
setRegisterInstr(0x60, 0x10);
interpreter.cycle();
setRegisterInstr(0x61, 0x15);
interpreter.cycle();
setRegisterInstr(0x80, 0x15);
interpreter.cycle();

EXPECT_EQ(interpreter.v[0], 0xFF - 0x4);
EXPECT_EQ(interpreter.v[0xF], 0);
EXPECT_EQ(interpreter.cir, 0x8015);
}

TEST_F(InterpreterTest, instruction8xy5Vf)
{
setRegisterInstr(0x60, 0x15);
interpreter.cycle();
setRegisterInstr(0x61, 0x10);
interpreter.cycle();
setRegisterInstr(0x80, 0x15);
interpreter.cycle();

EXPECT_EQ(interpreter.v[0], 0x5);
EXPECT_EQ(interpreter.v[0xF], 1);
EXPECT_EQ(interpreter.cir, 0x8015);
}

TEST_F(InterpreterTest, instruction8xy6NoVf)
{
setRegisterInstr(0x60, 0x10);
interpreter.cycle();
setRegisterInstr(0x61, 0xE);
interpreter.cycle();
setRegisterInstr(0x80, 0x16);
interpreter.cycle();

EXPECT_EQ(interpreter.v[0], 0x7);
EXPECT_EQ(interpreter.v[0xF], 0);
EXPECT_EQ(interpreter.cir, 0x8016);
}

TEST_F(InterpreterTest, instruction8xy6Vf)
{
setRegisterInstr(0x60, 0x10);
interpreter.cycle();
setRegisterInstr(0x61, 0xF);
interpreter.cycle();
setRegisterInstr(0x80, 0x16);
interpreter.cycle();

EXPECT_EQ(interpreter.v[0], 0x7);
EXPECT_EQ(interpreter.v[0xF], 1);
EXPECT_EQ(interpreter.cir, 0x8016);
}

TEST_F(InterpreterTest, instruction8xy7NoVf)
{
setRegisterInstr(0x60, 0x10);
interpreter.cycle();
setRegisterInstr(0x61, 0x15);
interpreter.cycle();
setRegisterInstr(0x80, 0x17);
interpreter.cycle();

EXPECT_EQ(interpreter.v[0], 0x5);
EXPECT_EQ(interpreter.v[0xF], 1);
EXPECT_EQ(interpreter.cir, 0x8017);
}

TEST_F(InterpreterTest, instruction8xy7Vf)
{
setRegisterInstr(0x60, 0x15);
interpreter.cycle();
setRegisterInstr(0x61, 0x10);
interpreter.cycle();
setRegisterInstr(0x80, 0x17);
interpreter.cycle();

EXPECT_EQ(interpreter.v[0], 0xFF - 0x4);
EXPECT_EQ(interpreter.v[0xF], 0);
EXPECT_EQ(interpreter.cir, 0x8017);
}

TEST_F(InterpreterTest, instruction8xyENoVf)
{
setRegisterInstr(0x60, 0x10);
interpreter.cycle();
setRegisterInstr(0x61, 0x7);
interpreter.cycle();
setRegisterInstr(0x80, 0x1E);
interpreter.cycle();

EXPECT_EQ(interpreter.v[0], 0xE);
EXPECT_EQ(interpreter.v[0xF], 0);
EXPECT_EQ(interpreter.cir, 0x801E);
}

TEST_F(InterpreterTest, instruction8xyEVf)
{
setRegisterInstr(0x60, 0x10);
interpreter.cycle();
setRegisterInstr(0x61, 0xFF);
interpreter.cycle();
setRegisterInstr(0x80, 0x1E);
interpreter.cycle();

EXPECT_EQ(interpreter.v[0], 0xFE);
EXPECT_EQ(interpreter.v[0xF], 1);
EXPECT_EQ(interpreter.cir, 0x801E);
}

TEST_F(InterpreterTest, instruction9xy0Skip)
{
setRegisterInstr(0x60, 0x44);
interpreter.cycle();
setRegisterInstr(0x61, 0x40);
interpreter.cycle();
setRegisterInstr(0x90, 0x10);
interpreter.cycle();

EXPECT_EQ(interpreter.pc, 0x208);
EXPECT_EQ(interpreter.cir, 0x9010);
}

TEST_F(InterpreterTest, instruction9xy0NoSkip)
{
setRegisterInstr(0x60, 0x44);
interpreter.cycle();
setRegisterInstr(0x61, 0x44);
interpreter.cycle();
setRegisterInstr(0x90, 0x10);
interpreter.cycle();

EXPECT_EQ(interpreter.pc, 0x206);
EXPECT_EQ(interpreter.cir, 0x9010);
}

TEST_F(InterpreterTest, instructionAnnn)
{
setRegisterInstr(0xAF, 0xFF);
interpreter.cycle();

EXPECT_EQ(interpreter.pc, 0x202);
EXPECT_EQ(interpreter.cir, 0xAFFF);
EXPECT_EQ(interpreter.i, 0xFFF);
}

TEST_F(InterpreterTest, instructionBnnn)
{
setRegisterInstr(0x60, 0x11);
interpreter.cycle();
setRegisterInstr(0xB0, 0x44);
interpreter.cycle();

EXPECT_EQ(interpreter.pc, 0x44 + 0x11);
EXPECT_EQ(interpreter.cir, 0xB044);
}

TEST_F(InterpreterTest, instructionCxnn)
{
setRegisterInstr(0xC0, 0xFF);
interpreter.cycle();

EXPECT_NE(interpreter.v[0], 0xFF);
EXPECT_EQ(interpreter.cir, 0xC0FF);
}

TEST_F(InterpreterTest, instructionFx07)
{
setRegisterInstr(0x60, 0xCC);
interpreter.cycle();

setRegisterInstr(0xF0, 0x15);
interpreter.cycle();

setRegisterInstr(0xF1, 0x07);
interpreter.cycle();

EXPECT_EQ(interpreter.v[0], 0xCC);
EXPECT_EQ(interpreter.v[1], 0xCC);
EXPECT_EQ(interpreter.dt, 0xCC);
EXPECT_EQ(interpreter.cir, 0xF107);
}

TEST_F(InterpreterTest, instructionFx15)
{
setRegisterInstr(0x60, 0xCC);
interpreter.cycle();

setRegisterInstr(0xF0, 0x15);
interpreter.cycle();

EXPECT_EQ(interpreter.v[0], 0xCC);
EXPECT_EQ(interpreter.dt, 0xCC);
EXPECT_EQ(interpreter.cir, 0xF015);
}

TEST_F(InterpreterTest, instructionFx18)
{
setRegisterInstr(0x60, 0xCC);
interpreter.cycle();

setRegisterInstr(0xF0, 0x18);
interpreter.cycle();

EXPECT_EQ(interpreter.v[0], 0xCC);
EXPECT_EQ(interpreter.st, 0xCC);
EXPECT_EQ(interpreter.cir, 0xF018);
}

TEST_F(InterpreterTest, instructionFx1E)
{
setRegisterInstr(0x60, 0xEE);
interpreter.cycle();

setRegisterInstr(0xF0, 0x1E);
interpreter.cycle();

EXPECT_EQ(interpreter.i, 0xEE);
EXPECT_EQ(interpreter.cir, 0xF01E);

setRegisterInstr(0x60, 0x1);
interpreter.cycle();

setRegisterInstr(0xF0, 0x1E);
interpreter.cycle();

EXPECT_EQ(interpreter.i, 0xEF);
EXPECT_EQ(interpreter.cir, 0xF01E);
}

TEST_F(InterpreterTest, instructionFx33)
{
setRegisterInstr(0x60, 0xEE);
interpreter.cycle();

setRegisterInstr(0xF0, 0x33);
interpreter.cycle();

EXPECT_EQ(memory.read(interpreter.i), 2);
EXPECT_EQ(memory.read(interpreter.i + 1), 3);
EXPECT_EQ(memory.read(interpreter.i + 2), 8);
EXPECT_EQ(interpreter.cir, 0xF033);
}

TEST_F(InterpreterTest, instructionFx55Single)
{
setRegisterInstr(0x60, 0xEE);
interpreter.cycle();

setRegisterInstr(0xF0, 0x55);
interpreter.cycle();

EXPECT_EQ(memory.read(interpreter.i - 1), 0xEE); // testing done for chip8 increment i quirk = true
EXPECT_EQ(interpreter.cir, 0xF055);
}

TEST_F(InterpreterTest, instructionFx55Multi)
{
setRegisterInstr(0x60, 0xEE);
interpreter.cycle();

setRegisterInstr(0x61, 0xFF);
interpreter.cycle();

setRegisterInstr(0xF1, 0x55);
interpreter.cycle();

EXPECT_EQ(memory.read(interpreter.i - 2), 0xEE);
EXPECT_EQ(memory.read(interpreter.i - 1), 0xFF);
EXPECT_EQ(interpreter.cir, 0xF155);
}

TEST_F(InterpreterTest, instructionFx65Single)
{
memory.write(static_cast<std::uint8_t>(84), static_cast<std::uint16_t>(1000));
setRegisterInstr(0xA3, 0xE8);
interpreter.cycle();

setRegisterInstr(0xF0, 0x65);
interpreter.cycle();

EXPECT_EQ(interpreter.v[0], 84);
EXPECT_EQ(interpreter.cir, 0xF065);
}

TEST_F(InterpreterTest, instructionFx65Muli)
{
memory.write(static_cast<std::uint8_t>(84), static_cast<std::uint16_t>(1000));
memory.write(static_cast<std::uint8_t>(88), static_cast<std::uint16_t>(1001));
setRegisterInstr(0xA3, 0xE8);
interpreter.cycle();

setRegisterInstr(0xF1, 0x65);
interpreter.cycle();

EXPECT_EQ(interpreter.v[0], 84);
EXPECT_EQ(interpreter.v[1], 88);
EXPECT_EQ(interpreter.cir, 0xF165);
}
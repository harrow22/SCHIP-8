#include <gtest/gtest.h>
#include "Cpu.h"

// Test suite for all opcodes minus input & display
class CpuTest : public testing::Test {
protected:
    void setRegisterInstr(std::uint8_t hi, std::uint8_t lo, std::uint16_t offset=0) {
        memory.write(hi, cpu.pc + offset);
        memory.write(lo, cpu.pc + offset + 1);
    }

    Memory memory {};
    Display display {};
    Keyboard keyboard {};
    Cpu cpu {memory, display, keyboard};
};

TEST_F(CpuTest, initialStateIsValid)
{
    EXPECT_EQ(cpu.pc, 0x200); // CHIP-8 programs start at memory addr 0x200
    EXPECT_EQ(cpu.cir, 0);
    EXPECT_EQ(cpu.i, 0);
    for (int i = 0; i != 16; ++i)
        EXPECT_EQ(cpu.reg[i], 0);
    EXPECT_EQ(cpu.sound, 0);
    EXPECT_EQ(cpu.delay, 0);
}

TEST_F(CpuTest, instructionCallAndRet)
{
    setRegisterInstr(0x24, 0x44);
    setRegisterInstr(0x00, 0xEE, 0x444 - 0x200);
    cpu.cycle();
    EXPECT_EQ(memory.stack.top(), 0x202);
    cpu.cycle();

    EXPECT_EQ(cpu.pc, 0x202);
    EXPECT_EQ(cpu.cir, 0x00EE);
}

TEST_F(CpuTest, instruction1nnn)
{
    setRegisterInstr(0x20, 0x44);
    cpu.cycle();

    EXPECT_EQ(cpu.pc, 0x044);
    EXPECT_EQ(cpu.cir, 0x2044);
}

TEST_F(CpuTest, instruction3xnnSkip)
{
    setRegisterInstr(0x60, 0x44);
    cpu.cycle();
    setRegisterInstr(0x30, 0x44);
    cpu.cycle();

    EXPECT_EQ(cpu.pc, 0x206);
    EXPECT_EQ(cpu.cir, 0x3044);
}

TEST_F(CpuTest, instruction3xnnNoSkip)
{
    setRegisterInstr(0x60, 0x44);
    cpu.cycle();
    setRegisterInstr(0x30, 0x42);
    cpu.cycle();

    EXPECT_EQ(cpu.pc, 0x204);
    EXPECT_EQ(cpu.cir, 0x3042);
}

TEST_F(CpuTest, instruction4xnnSkip)
{
    setRegisterInstr(0x60, 0x40);
    cpu.cycle();
    setRegisterInstr(0x40, 0x44);
    cpu.cycle();

    EXPECT_EQ(cpu.pc, 0x206);
    EXPECT_EQ(cpu.cir, 0x4044);
}

TEST_F(CpuTest, instruction4xnnNoSkip)
{
    setRegisterInstr(0x60, 0x44);
    cpu.cycle();
    setRegisterInstr(0x40, 0x44);
    cpu.cycle();

    EXPECT_EQ(cpu.pc, 0x204);
    EXPECT_EQ(cpu.cir, 0x4044);
}

TEST_F(CpuTest, instruction5xy0Skip)
{
    setRegisterInstr(0x60, 0x44);
    cpu.cycle();
    setRegisterInstr(0x61, 0x44);
    cpu.cycle();
    setRegisterInstr(0x50, 0x10);
    cpu.cycle();

    EXPECT_EQ(cpu.pc, 0x208);
    EXPECT_EQ(cpu.cir, 0x5010);
}

TEST_F(CpuTest, instruction5xy0NoSkip)
{
    setRegisterInstr(0x60, 0x44);
    cpu.cycle();
    setRegisterInstr(0x61, 0x42);
    cpu.cycle();
    setRegisterInstr(0x50, 0x10);
    cpu.cycle();

    EXPECT_EQ(cpu.pc, 0x206);
    EXPECT_EQ(cpu.cir, 0x5010);
}

TEST_F(CpuTest, instruction6xnn)
{
    setRegisterInstr(0x60, 0x12);
    cpu.cycle();

    EXPECT_EQ(cpu.pc, 0x202);
    EXPECT_EQ(cpu.cir, 0x6012);
    EXPECT_EQ(cpu.reg[0], 0x12);
}

TEST_F(CpuTest, instruction7xnn)
{
    setRegisterInstr(0x62, 0x12);
    cpu.cycle();

    setRegisterInstr(0x72, 0x32);
    cpu.cycle();

    EXPECT_EQ(cpu.pc, 0x204);
    EXPECT_EQ(cpu.cir, 0x7232);
    EXPECT_EQ(cpu.reg[2], 0x44);
}

TEST_F(CpuTest, instruction8xy0)
{
    setRegisterInstr(0x60, 0x01);
    cpu.cycle();
    setRegisterInstr(0x61, 0x02);
    cpu.cycle();
    setRegisterInstr(0x82, 0x10);
    cpu.cycle();

    EXPECT_EQ(cpu.reg[2], 2);
    EXPECT_EQ(cpu.cir, 0x8210);
}

TEST_F(CpuTest, instruction8xy1)
{
    setRegisterInstr(0x60, 0x01);
    cpu.cycle();
    setRegisterInstr(0x61, 0x02);
    cpu.cycle();
    setRegisterInstr(0x80, 0x11);
    cpu.cycle();

    EXPECT_EQ(cpu.reg[0], 3);
    EXPECT_EQ(cpu.cir, 0x8011);
}

TEST_F(CpuTest, instruction8xy2)
{
    setRegisterInstr(0x60, 0x01);
    cpu.cycle();
    setRegisterInstr(0x61, 0x02);
    cpu.cycle();
    setRegisterInstr(0x80, 0x12);
    cpu.cycle();

    EXPECT_EQ(cpu.reg[0], 0);
    EXPECT_EQ(cpu.cir, 0x8012);
}

TEST_F(CpuTest, instruction8xy3)
{
    setRegisterInstr(0x60, 0x01);
    cpu.cycle();
    setRegisterInstr(0x61, 0x02);
    cpu.cycle();
    setRegisterInstr(0x80, 0x13);
    cpu.cycle();

    EXPECT_EQ(cpu.reg[0], 3);
    EXPECT_EQ(cpu.cir, 0x8013);
}

TEST_F(CpuTest, instruction8xy4NoVf)
{
    setRegisterInstr(0x60, 0xB);
    cpu.cycle();
    setRegisterInstr(0x61, 0xA);
    cpu.cycle();
    setRegisterInstr(0x80, 0x14);
    cpu.cycle();

    EXPECT_EQ(cpu.reg[0], 0x15);
    EXPECT_EQ(cpu.reg[0xF], 0);
    EXPECT_EQ(cpu.cir, 0x8014);
}

TEST_F(CpuTest, instruction8xy4Vf)
{
    setRegisterInstr(0x60, 0x7F);
    cpu.cycle();
    setRegisterInstr(0x61, 0x85);
    cpu.cycle();
    setRegisterInstr(0x80, 0x14);
    cpu.cycle();

    EXPECT_EQ(cpu.reg[0], 4);
    EXPECT_EQ(cpu.reg[0xF], 1);
    EXPECT_EQ(cpu.cir, 0x8014);
}

TEST_F(CpuTest, instruction8xy5NoVf)
{
    setRegisterInstr(0x60, 0x10);
    cpu.cycle();
    setRegisterInstr(0x61, 0x15);
    cpu.cycle();
    setRegisterInstr(0x80, 0x15);
    cpu.cycle();

    EXPECT_EQ(cpu.reg[0], 0xFF - 0x4);
    EXPECT_EQ(cpu.reg[0xF], 0);
    EXPECT_EQ(cpu.cir, 0x8015);
}

TEST_F(CpuTest, instruction8xy5Vf)
{
    setRegisterInstr(0x60, 0x15);
    cpu.cycle();
    setRegisterInstr(0x61, 0x10);
    cpu.cycle();
    setRegisterInstr(0x80, 0x15);
    cpu.cycle();

    EXPECT_EQ(cpu.reg[0], 0x5);
    EXPECT_EQ(cpu.reg[0xF], 1);
    EXPECT_EQ(cpu.cir, 0x8015);
}

TEST_F(CpuTest, instruction8xy6NoVf)
{
    setRegisterInstr(0x60, 0x10);
    cpu.cycle();
    setRegisterInstr(0x61, 0xE);
    cpu.cycle();
    setRegisterInstr(0x80, 0x16);
    cpu.cycle();

    EXPECT_EQ(cpu.reg[0], 0x7);
    EXPECT_EQ(cpu.reg[0xF], 0);
    EXPECT_EQ(cpu.cir, 0x8016);
}

TEST_F(CpuTest, instruction8xy6Vf)
{
    setRegisterInstr(0x60, 0x10);
    cpu.cycle();
    setRegisterInstr(0x61, 0xF);
    cpu.cycle();
    setRegisterInstr(0x80, 0x16);
    cpu.cycle();

    EXPECT_EQ(cpu.reg[0], 0x7);
    EXPECT_EQ(cpu.reg[0xF], 1);
    EXPECT_EQ(cpu.cir, 0x8016);
}

TEST_F(CpuTest, instruction8xy7NoVf)
{
    setRegisterInstr(0x60, 0x10);
    cpu.cycle();
    setRegisterInstr(0x61, 0x15);
    cpu.cycle();
    setRegisterInstr(0x80, 0x17);
    cpu.cycle();

    EXPECT_EQ(cpu.reg[0], 0x5);
    EXPECT_EQ(cpu.reg[0xF], 1);
    EXPECT_EQ(cpu.cir, 0x8017);
}

TEST_F(CpuTest, instruction8xy7Vf)
{
    setRegisterInstr(0x60, 0x15);
    cpu.cycle();
    setRegisterInstr(0x61, 0x10);
    cpu.cycle();
    setRegisterInstr(0x80, 0x17);
    cpu.cycle();

    EXPECT_EQ(cpu.reg[0], 0xFF - 0x4);
    EXPECT_EQ(cpu.reg[0xF], 0);
    EXPECT_EQ(cpu.cir, 0x8017);
}

TEST_F(CpuTest, instruction8xyENoVf)
{
    setRegisterInstr(0x60, 0x10);
    cpu.cycle();
    setRegisterInstr(0x61, 0x7);
    cpu.cycle();
    setRegisterInstr(0x80, 0x1E);
    cpu.cycle();

    EXPECT_EQ(cpu.reg[0], 0xE);
    EXPECT_EQ(cpu.reg[0xF], 0);
    EXPECT_EQ(cpu.cir, 0x801E);
}

TEST_F(CpuTest, instruction8xyEVf)
{
    setRegisterInstr(0x60, 0x10);
    cpu.cycle();
    setRegisterInstr(0x61, 0xFF);
    cpu.cycle();
    setRegisterInstr(0x80, 0x1E);
    cpu.cycle();

    EXPECT_EQ(cpu.reg[0], 0xFE);
    EXPECT_EQ(cpu.reg[0xF], 1);
    EXPECT_EQ(cpu.cir, 0x801E);
}

TEST_F(CpuTest, instruction9xy0Skip)
{
    setRegisterInstr(0x60, 0x44);
    cpu.cycle();
    setRegisterInstr(0x61, 0x40);
    cpu.cycle();
    setRegisterInstr(0x90, 0x10);
    cpu.cycle();

    EXPECT_EQ(cpu.pc, 0x208);
    EXPECT_EQ(cpu.cir, 0x9010);
}

TEST_F(CpuTest, instruction9xy0NoSkip)
{
    setRegisterInstr(0x60, 0x44);
    cpu.cycle();
    setRegisterInstr(0x61, 0x44);
    cpu.cycle();
    setRegisterInstr(0x90, 0x10);
    cpu.cycle();

    EXPECT_EQ(cpu.pc, 0x206);
    EXPECT_EQ(cpu.cir, 0x9010);
}

TEST_F(CpuTest, instructionAnnn)
{
    setRegisterInstr(0xAF, 0xFF);
    cpu.cycle();

    EXPECT_EQ(cpu.pc, 0x202);
    EXPECT_EQ(cpu.cir, 0xAFFF);
    EXPECT_EQ(cpu.i, 0xFFF);
}

TEST_F(CpuTest, instructionBnnn)
{
    setRegisterInstr(0x60, 0x11);
    cpu.cycle();
    setRegisterInstr(0xB0, 0x44);
    cpu.cycle();

    EXPECT_EQ(cpu.pc, 0x44 + 0x11);
    EXPECT_EQ(cpu.cir, 0xB044);
}

TEST_F(CpuTest, instructionCxnn)
{
    setRegisterInstr(0xC0, 0xFF);
    cpu.cycle();

    EXPECT_NE(cpu.reg[0], 0xFF);
    EXPECT_EQ(cpu.cir, 0xC0FF);
}

TEST_F(CpuTest, instructionFx07)
{
    setRegisterInstr(0x60, 0xCC);
    cpu.cycle();

    setRegisterInstr(0xF0, 0x15);
    cpu.cycle();

    setRegisterInstr(0xF1, 0x07);
    cpu.cycle();

    EXPECT_EQ(cpu.reg[0], 0xCC);
    EXPECT_EQ(cpu.reg[1], 0xCC);
    EXPECT_EQ(cpu.delay, 0xCC);
    EXPECT_EQ(cpu.cir, 0xF107);
}

TEST_F(CpuTest, instructionFx15)
{
    setRegisterInstr(0x60, 0xCC);
    cpu.cycle();

    setRegisterInstr(0xF0, 0x15);
    cpu.cycle();

    EXPECT_EQ(cpu.reg[0], 0xCC);
    EXPECT_EQ(cpu.delay, 0xCC);
    EXPECT_EQ(cpu.cir, 0xF015);
}

TEST_F(CpuTest, instructionFx18)
{
    setRegisterInstr(0x60, 0xCC);
    cpu.cycle();

    setRegisterInstr(0xF0, 0x18);
    cpu.cycle();

    EXPECT_EQ(cpu.reg[0], 0xCC);
    EXPECT_EQ(cpu.sound, 0xCC);
    EXPECT_EQ(cpu.cir, 0xF018);
}

TEST_F(CpuTest, instructionFx1E)
{
    setRegisterInstr(0x60, 0xEE);
    cpu.cycle();

    setRegisterInstr(0xF0, 0x1E);
    cpu.cycle();

    EXPECT_EQ(cpu.i, 0xEE);
    EXPECT_EQ(cpu.cir, 0xF01E);

    setRegisterInstr(0x60, 0x1);
    cpu.cycle();

    setRegisterInstr(0xF0, 0x1E);
    cpu.cycle();

    EXPECT_EQ(cpu.i, 0xEF);
    EXPECT_EQ(cpu.cir, 0xF01E);
}

TEST_F(CpuTest, instructionFx33)
{
    setRegisterInstr(0x60, 0xEE);
    cpu.cycle();

    setRegisterInstr(0xF0, 0x33);
    cpu.cycle();

    EXPECT_EQ(memory.read(cpu.i), 2);
    EXPECT_EQ(memory.read(cpu.i + 1), 3);
    EXPECT_EQ(memory.read(cpu.i + 2), 8);
    EXPECT_EQ(cpu.cir, 0xF033);
}

TEST_F(CpuTest, instructionFx55Single)
{
    setRegisterInstr(0x60, 0xEE);
    cpu.cycle();

    setRegisterInstr(0xF0, 0x55);
    cpu.cycle();

    EXPECT_EQ(memory.read(cpu.i), 0xEE);
    EXPECT_EQ(cpu.cir, 0xF055);
}

TEST_F(CpuTest, instructionFx55Multi)
{
    setRegisterInstr(0x60, 0xEE);
    cpu.cycle();

    setRegisterInstr(0x61, 0xFF);
    cpu.cycle();

    setRegisterInstr(0xF1, 0x55);
    cpu.cycle();

    EXPECT_EQ(memory.read(cpu.i), 0xEE);
    EXPECT_EQ(memory.read(cpu.i + 1), 0xFF);
    EXPECT_EQ(cpu.cir, 0xF155);
}

TEST_F(CpuTest, instructionFx65Single)
{
    memory.write(static_cast<std::uint8_t>(84), static_cast<std::uint16_t>(1000));
    setRegisterInstr(0xA3, 0xE8);
    cpu.cycle();

    setRegisterInstr(0xF0, 0x65);
    cpu.cycle();

    EXPECT_EQ(cpu.reg[0], 84);
    EXPECT_EQ(cpu.cir, 0xF065);
}

TEST_F(CpuTest, instructionFx65Muli)
{
    memory.write(static_cast<std::uint8_t>(84), static_cast<std::uint16_t>(1000));
    memory.write(static_cast<std::uint8_t>(88), static_cast<std::uint16_t>(1001));
    setRegisterInstr(0xA3, 0xE8);
    cpu.cycle();

    setRegisterInstr(0xF1, 0x65);
    cpu.cycle();

    EXPECT_EQ(cpu.reg[0], 84);
    EXPECT_EQ(cpu.reg[1], 88);
    EXPECT_EQ(cpu.cir, 0xF165);
}
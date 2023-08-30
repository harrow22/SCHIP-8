#include <gtest/gtest.h>
#include "Memory.h"

class MemoryTest : public testing::Test {
protected:
    MemoryTest() {
        memory.write(static_cast<std::uint8_t>(0xF), static_cast<std::uint16_t>(0x200));
        memory.write(static_cast<std::uint8_t>(0xFF), static_cast<std::uint16_t>(0x201));
        memory.write(static_cast<std::uint8_t>(0x11), static_cast<std::uint16_t>(0x202));
    }
    Memory memory {};
};

TEST_F(MemoryTest, readWorks)
{
    EXPECT_EQ(memory.read(0x200), 0xF);
    EXPECT_EQ(memory.read(0x201), 0xFF);
    EXPECT_EQ(memory.read(0x202), 0x11);
    EXPECT_EQ(memory.read(0x203), 0);
}

TEST_F(MemoryTest, writeWorks)
{
    memory.write(static_cast<std::uint8_t>(0xAA), static_cast<std::uint16_t>(0x203));
    EXPECT_EQ(memory.read(0x203), 0xAA);
}

TEST_F(MemoryTest, outOfBoundsTesting)
{
    EXPECT_THROW(memory.read(-1), std::out_of_range);
    EXPECT_THROW(memory.read(4096), std::out_of_range);
}

TEST_F(MemoryTest, resetWorks)
{
    memory.write(static_cast<std::uint8_t>(0xAA), static_cast<std::uint16_t>(4095));
    memory.wipe();
    EXPECT_EQ(memory.read(0x200), 0);
    EXPECT_EQ(memory.read(4095), 0);
}
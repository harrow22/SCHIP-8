#include "Memory.h"

void Memory::wipe() {
    for (auto i = 0x200; i != MEM_SIZE; ++i)
        arr[i] = 0;
}

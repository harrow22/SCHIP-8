# CHIP-8/SUPER-CHIP Emulator
<p align="center">
  <img alt="Spacefight 2091 demo gif" src="https://github.com/harrow22/SCHIP-8/blob/664e7d586bb251e591e8996cd9576ea44ca9a79b/examples/demo.gif" />
</p>

A CHIP-8 and SUPER-CHIP emulator written in C++. All opcodes and features of both interpreters implemented (excluding
a st playing when the `st timer` register is `>0`).

## Table of Contents
* [Installation](#installation)
  * [Dependencies](#dependencies)
  * [Build](#build)
* [Usage](#usage)
  * [Command line arguments](#command-line-arguments)
  * [Quirk flags](#quirk-flags)
  * [Debugger](#debugger)
  * [Running tests](#running-tests)
  * [Changing colors](#changing-colors)
* [Thanks](#thanks)

# Installation
### Dependencies
You will need,
* **A C++ Compiler**
* **CMake Version 3.26+**
* **A CMake Build Generator**

The [**SDL2**](https://github.com/libsdl-org/SDL) and [**GoogleTest**](https://github.com/google/googletest) frameworks will be made available to the program with the use of CMake's
``FetchContent_MakeAvailable`` command.

### Building with CMake
```
$ git clone https://github.com/harrow22/SCHIP-8.git
$ cd SCHIP-8
$ cmake -S . -B build -G your_generator -DCMAKE_BUILD_TYPE=RELEASE
$ cmake --build build
```

> [!NOTE]
> I tested the emulator on Windows 11 using MinGW and Ninja.

# Usage
**To run the emulator,**
```
$ cd build/src/
$ SCHIP-8.exe <ARGUMENTS>...
```
> [!IMPORTANT]
> The emulator can't run without passing the `rom` argument.

### Command line arguments

* `-rom <path/to/rom>` - Specifies a path to the rom you want to run on the emulator.
* `-cycles_per_frame <number>` - Controls how fast the emulator runs. The higher the number, the faster the emulator.
  *By default*, the emulator runs at **20 cycles per frame**. I encourage you to play around with this number, if the feel
  of the emulation is off.
* `-superchip` - This will enable the SUPER-CHIP [quirk flags](#quirk-flags).
* `-quirk <quirk_name=bool>` - Used to toggle a specific quirk on or off.
* `-debug` - The emulator will start running immediately in [debug mode](#debugger).
* `-help` - Will print a list of supported command line arguments to the console.

> [!WARNING]
> #### Argument Order Matters
> If both `-superchip` and `-quirk` arguments are specified. The argument coming *last* will **override** the configuration of the
preceding argument.

> [!NOTE]
> The actual arguments should not contain the symbols `<` or `>`.

#### Quirk flags
Quirk flags are used to toggle differing behavior arising from the various CHIP-8 implementations/extensions
(e.g. SUPER-CHIP). By default, the emulator is configured to work like the VIP CHIP-8 interpreter.

* `vf_reset` - By `DEFAULT=TRUE`, set register `Vf` to `0` after `vx |= vy`, `vx &= vy`, and `vx ^= vy`.
* `memory` - By `DEFAULT=TRUE`, opcodes that load from and store to memory will increment register `i`.
* `shifting` - By `DEFAULT=FALSE`, opcodes using `<<=` and `>>=` will modify register `Vx` in place and ignore
  register `Vy`.
* `jumping` - By `DEFAULT=FALSE`, jp_ with offset will use the value of register `Vx` instead of the 4 left-most
  bits of the target address.
* `ioverflow` - By `DEFAULT=FALSE`, set register `Vf` to `0` on overflow of `I = I + Vx` (greater than `0x1000`). Relied
  on by at least one known game *Spacefight 2091*

### Debugger
The debugger outputs the current state of the registers, the opcode just executed and the current cycle
number to the console. Using it is simple,

Press `I` to enter/exit_ debug mode.

Press `O` to cycle the interpreter.

See [command line arguments](#command-line-arguments) to enter the debugger immediately on launch of the emulator.

### Running Tests
I used Google's GoogleTest framework to run unit tests during development. You can run them by,
```
$ cd tests/
$ ctest
```
For a more detailed report, the test logs are written to `tests/Testing/Temporary/` or you can,
```
$ cat Testing/Temporary/LastTestLog.txt
```

### Changing Colors
You can modify the colors of the emulator if you wish. The macros for them (`HEX_COLOR_ON` and `HEX_COLOR_OFF`)
are defined in the [display header](/src/display/Display.h).

## Thanks
##### [CHIP-8 Test Suite](https://github.com/Timendus/chip8-test-suite)<br>
##### [CHIP-8 Documentation](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)<br>
##### [Mastering SuperChip](http://johnearnest.github.io/Octo/docs/SuperChip.html)

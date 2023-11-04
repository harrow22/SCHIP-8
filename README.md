# CHIP-8/SUPER-CHIP Emulator
<p align="center">
  <img alt="Spacefight 2091 demo gif" src="https://raw.githubusercontent.com/harrow22/SCHIP-8/master/assets/demo.gif" />
</p>

Another CHIP-8 and SUPER-CHIP emulator written in C++. Sound not implemented.

## Table of Contents
* [Usage](#installation)
  * [Dependencies](#dependencies)
  * [Build](#build)
  * [Command line arguments](#command-line-arguments)
    * [Quirk flags](#quirk-flags)
  * [Changing colors](#changing-colors)
  * [Debugger](#debugger)
* [Running tests](#running-tests)
* [Thanks](#thanks)

## Usage
### Dependencies
You will need,
* **A C++ Compiler**
* **CMake Version 3.26+**
* **A CMake Build Generator**
* **[SDL2](https://github.com/libsdl-org/SDL)**

### With CMake
```
git clone https://github.com/harrow22/SCHIP-8.git
cd SCHIP-8
cmake -S . -B build -G your_generator -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_PREFIX_PATH=path/to/SDL2install
cmake --build build
build/src/SCHIP-8.exe <ARGUMENTS>...
```

> [!IMPORTANT]
> The emulator won't run without being provided a rom.

### Command line arguments
* `-rom <path/to/rom>` - This is the rom the emulator will play. Must be specified.
* `-cycles_per_frame <number>` - Controls the speed at which the emulator runs (default = 20). Changing it can help improve the "feel" of certain roms.
* `--mode <type>` - Allows either: `superchip`, `xochip`, or `default` (optional). This option will override all quirk flags except `ioverflow`
* `-quirk <quirk_name=bool>` - Used to toggle a specific quirk on or off.
* `-debug` - The emulator will start running immediately in [debug mode](#debugger).

#### Quirk flags
Quirk flags are used to toggle different implementation details from the various interpreters. Defaults are shown after the equal sign.

* `vf_reset=true` - set register `Vf` to `0` after `vx |= vy`, `vx &= vy`, and `vx ^= vy`.
* `memory=true` - opcodes that load from and store to memory will increment register `i`.
* `shifting=false` - opcodes using `<<=` and `>>=` will modify register `Vx` in place and ignore register `Vy`.
* `jumping=false` - jp_ with offset will use the value of register `Vx` instead of the 4 left-most bits of the target address.
* `ioverflow=false` - set register `Vf` to `0` on ioverflow of `I = I + Vx` (greater than `0x1000`). Apparently used by at least one game: *Spacefight 2091*

### Changing Colors
You can modify the colors of the emulator if you wish. The constants for them (`colorOn` and `colorOff`) are defined in the [display header](/src/display/Display.h). Just append `FF` to whichever hex code you choose (e.g. the default on color is `#9C5ECC` + `FF` = `0x9C5ECCFF`).

### Debugger
The debugger outputs the current state of the registers, the opcode just executed and the current cycle
number to the console. Using it is simple,

Press `I` to enter/exit_ debug mode.

Press `O` to step the interpreter forward once.

See [command line arguments](#command-line-arguments) to enter the debugger immediately on launch of the emulator.

## Running Tests
I used Google's GoogleTest framework to run unit tests during development. If you would like to use them, you will need to rebuild the project with testing enabled.
```
cmake -S . -B build -G your_generator -DTESTING=ON -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_PREFIX_PATH=path/to/SDL2install
cmake --build build
cd build/tests/
ctest
```

For a more detailed report, the test logs are written to `tests/Testing/Temporary/` or you can use the command,
```
$ cat Testing/Temporary/LastTestLog.txt
```

## Thanks
* Timendus's [CHIP-8 Test Suite](https://github.com/Timendus/chip8-test-suite) was very helpful during development
* tobiasvl's [CHIP-8 documentation](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)
* John Earnest's [Mastering SuperChip](http://johnearnest.github.io/Octo/docs/SuperChip.html) blog post

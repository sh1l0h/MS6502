# MS6502
MS6502(Microprocessor Simulator 6502) is designed to emulate the behavior of [the 6502 processor](https://en.wikipedia.org/wiki/MOS_Technology_6502), which was widely used in many popular home computers and gaming consoles during the 1980s.

## Tester
[tester](./tests/tester.c) is an essential tool for testing the emulator's functionality and ensuring that it operates correctly. It works by running programs on the emulator step by step and comparing the results with the expected ones.

To use the tester, you need to provide it with an input file. Each line of the input file that does not start with ':' is ignored. Lines that start with ':' must be formatted as follows:
'''
:number_of_clock_signals:binary_path: register value, 'MEM' address value, ...
'''
Where:
- **number_of_clock_signals** represents the number of clock signals to run the program for. If the value starts with '+', the state of the CPU after the previous test is preserved.
- **binary_path** represents the path to a file containing raw binary code that is loaded into the emulator's RAM. If binary_path is '_', the RAM is preserved from the previous test.
- **register** can be any of the following: 'PC', 'AC', 'X', 'Y', 'SR', 'SP', or 'IR'. **value** is the expected value of the specified register after running the program.
- **'MEM' address value** is a special case that represents the memory cell at the specified **address**, and **value** is the expected value of that cell after running the program.

Here's an example of how to use the tester:
'''
:3:test_program: PC 0x0200, AC 0x01, X 0x02, Y 0x03, SR 0x20, SP 0xff, MEM 0x0300 0x0a, MEM 0x0301 0x0b
'''
This example tests a program stored in the file 'test_program' running it for 3 clock cycles. It then checks the expected values of the CPU registers and memory locations. Specifically, it expects the program counter ('PC') to be 0x0200, the accumulator ('AC') to be 0x01, the X and Y registers to be 0x02 and 0x03, respectively, the status register ('SR') to be 0x20, the stack pointer ('SP') to be 0xff, and the values at memory locations 0x0300 and 0x0301 to be 0x0a and 0x0b, respectively.
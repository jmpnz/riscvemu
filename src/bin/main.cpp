#include <_types/_uint8_t.h>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "Instructions.h"
#include "Machine.h"

auto main(int argc, char* argv[]) -> int {

    if (argc < 2) {
        std::cout << "Usage: riscvemu file.bin" << '\n';
        return -1;
    }

    std::cout << argv[1] << '\n';
    std::vector<std::string> const msg{
        "Hello", "C++", "World", "from", "VS Code", "and the C++ extension!"};

    for (const std::string& word : msg) {
        std::cout << word << " ";
    }
    std::cout << std::endl;
    /// @example
    /// add two constants
    /// addi x29, x0, 5 // Add 5 and 0 store the value to x29
    /// addi x30, x0, 37 // Add 37 and 0 store the value in x30
    /// add x31, x30, x29 // Add x29 and x30 and store the value in x31
    /// 00500e93
    std::vector<uint8_t> const code{
        0x00,
        0x50,
        0x0e,
        0x93,
    };
    std::ifstream input(argv[1], std::ios::binary);
    std::vector<uint8_t> const buffer(std::istreambuf_iterator<char>(input),
                                      {});
    auto ctx = riscvemu::VMContext(buffer);
    auto cpu = riscvemu::CPU(ctx);

    cpu.DumpRegisters();

    cpu.Run();

    cpu.DumpRegisters();
}

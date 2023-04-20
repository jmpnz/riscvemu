#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <iostream>
#include <stdexcept>

#include "Decoder.h"
#include "Instructions.h"
#include "doctest.h"

TEST_CASE("testing instruction decoding") {
    // addi x1, x2, 48
    uint32_t instruction = 0x03010093;

    auto decoded = riscvemu::Itype(instruction);

    CHECK(decoded.Imm == 0x30);
    CHECK(decoded.Rd == riscvemu::Register(1));
    CHECK(decoded.Rs1 == riscvemu::Register(2));

    // Decode manually
    auto decodedT = riscvemu::decodeInstruction<riscvemu::Itype>(instruction);
    CHECK(decodedT.Imm == 0x30);
}

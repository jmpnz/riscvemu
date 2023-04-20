#include "Machine.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "Decoder.h"
#include "Instructions.h"
#include "doctest.h"

auto setupTestContext(const char* filename) -> riscvemu::CPU {
    std::ifstream input(filename, std::ios::binary);
    std::vector<uint8_t> const buffer(std::istreambuf_iterator<char>(input),
                                      {});
    auto ctx = riscvemu::VMContext(buffer);
    auto cpu = riscvemu::CPU(ctx);

    return cpu;
}

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

TEST_CASE("testing addi instruction") {
    const auto* fp = "addi.bin";
    auto cpu       = setupTestContext(fp);
    cpu.run();
    CHECK(cpu.getRegister(riscvemu::Register::T6) == 42);
}

TEST_CASE("testing lui instruction") {
    const auto* fp = "lui.bin";
    auto cpu       = setupTestContext(fp);
    cpu.run();
    CHECK(cpu.getRegister(riscvemu::Register::A0) == (42 << 12));
}

TEST_CASE("testing auipc instruction") {
    const auto* const fp = "auipc.bin";
    auto cpu             = setupTestContext(fp);
    cpu.run();
    CHECK(cpu.getRegister(riscvemu::Register::A0) ==
          riscvemu::MemoryBaseAddr + (42 << 12));
}

TEST_CASE("testing jal instruction") {
    const auto* const fp = "jal.bin";
    auto cpu             = setupTestContext(fp);
    cpu.run();
    CHECK(cpu.getRegister(riscvemu::Register::A0) ==
          riscvemu::MemoryBaseAddr + 4);

    CHECK(cpu.getPC() == riscvemu::MemoryBaseAddr + 42);
}

TEST_CASE("testing jalr instruction") {
    const auto* const fp = "jalr.bin";
    auto cpu             = setupTestContext(fp);
    cpu.run();
    CHECK(cpu.getRegister(riscvemu::Register::A0) ==
          riscvemu::MemoryBaseAddr + 8);

    CHECK(cpu.getPC() == 34);
}

TEST_CASE("testing beq instruction") {
    const auto* const fp = "beq.bin";
    auto cpu             = setupTestContext(fp);
    cpu.run();
    CHECK(cpu.getPC() == riscvemu::MemoryBaseAddr + 42);
}

TEST_CASE("testing bne instruction") {
    const auto* const fp = "bne.bin";
    auto cpu             = setupTestContext(fp);
    cpu.run();
    CHECK(cpu.getRegister(riscvemu::Register::Ra) == 10);
    CHECK(cpu.getPC() == riscvemu::MemoryBaseAddr + 42);
}

TEST_CASE("testing blt instruction") {
    const auto* const fp = "blt.bin";
    auto cpu             = setupTestContext(fp);
    cpu.run();
    CHECK(cpu.getRegister(riscvemu::Register::Ra) == 10);
    CHECK(cpu.getRegister(riscvemu::Register::Sp) == 20);
    CHECK(cpu.getPC() == riscvemu::MemoryBaseAddr + 42);
}

TEST_CASE("testing slt instruction") {
    const auto* fp = "slt.bin";
    auto cpu       = setupTestContext(fp);
    cpu.run();

    CHECK(cpu.getRegister(riscvemu::Register::T2) == 1);
    CHECK(cpu.getRegister(riscvemu::Register::T3) == 1);
    CHECK(cpu.getRegister(riscvemu::Register::T4) == 1);
}

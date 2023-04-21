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

TEST_CASE("testing bge instruction") {
    const auto* const fp = "bge.bin";
    auto cpu             = setupTestContext(fp);
    cpu.run();
    CHECK(cpu.getRegister(riscvemu::Register::Ra) == 10);
    CHECK(cpu.getRegister(riscvemu::Register::Sp) == 20);
    CHECK(cpu.getPC() == riscvemu::MemoryBaseAddr + 42);
}

TEST_CASE("testing bltu instruction") {
    const auto* const fp = "bltu.bin";
    auto cpu             = setupTestContext(fp);
    cpu.run();
    CHECK(cpu.getRegister(riscvemu::Register::Ra) == 10);
    CHECK(cpu.getRegister(riscvemu::Register::Sp) == 20);
    CHECK(cpu.getPC() == riscvemu::MemoryBaseAddr + 42);
}

TEST_CASE("testing bgeu instruction") {
    const auto* const fp = "bgeu.bin";
    auto cpu             = setupTestContext(fp);
    cpu.run();
    CHECK(cpu.getRegister(riscvemu::Register::Ra) == 10);
    CHECK(cpu.getRegister(riscvemu::Register::Sp) == 20);
    CHECK(cpu.getPC() == riscvemu::MemoryBaseAddr + 42);
}

TEST_CASE("testing lw instruction") {
    const auto* const fp = "load_store.bin";
    auto cpu             = setupTestContext(fp);
    cpu.run();
    CHECK(cpu.getRegister(riscvemu::Register::T1) == 0);
    CHECK(cpu.getRegister(riscvemu::Register::T2) == 256);
}

TEST_CASE("testing slt instruction") {
    const auto* fp = "slt.bin";
    auto cpu       = setupTestContext(fp);
    cpu.run();

    CHECK(cpu.getRegister(riscvemu::Register::T2) == 1);
    CHECK(cpu.getRegister(riscvemu::Register::T3) == 1);
    CHECK(cpu.getRegister(riscvemu::Register::T4) == 1);
}

TEST_CASE("testing xor instruction") {
    const auto* fp = "xor.bin";
    auto cpu       = setupTestContext(fp);
    cpu.run();

    CHECK(cpu.getRegister(riscvemu::Register::A1) == 3);
    CHECK(cpu.getRegister(riscvemu::Register::A2) == 0);
}

TEST_CASE("testing or instruction") {
    const auto* fp = "or.bin";
    auto cpu       = setupTestContext(fp);
    cpu.run();

    CHECK(cpu.getRegister(riscvemu::Register::A1) == 3);
    CHECK(cpu.getRegister(riscvemu::Register::A2) == 2);
}

TEST_CASE("testing and instruction") {
    const auto* fp = "and.bin";
    auto cpu       = setupTestContext(fp);
    cpu.run();

    CHECK(cpu.getRegister(riscvemu::Register::A1) == 2);
    CHECK(cpu.getRegister(riscvemu::Register::A2) == 2);
}

TEST_CASE("testing sll instruction") {
    const auto* fp = "sll.bin";
    auto cpu       = setupTestContext(fp);
    cpu.run();

    CHECK(cpu.getRegister(riscvemu::Register::A2) == 1 << 5);
    CHECK(cpu.getRegister(riscvemu::Register::A3) == 1 << 5);
    CHECK(cpu.getRegister(riscvemu::Register::A4) == 1);
}

TEST_CASE("testing sra instruction") {
    const auto* fp = "sra.bin";
    auto cpu       = setupTestContext(fp);
    cpu.run();

    CHECK(cpu.getRegister(riscvemu::Register::A2) == (uint64_t)(int64_t)-4);
    CHECK(cpu.getRegister(riscvemu::Register::A3) == (uint64_t)(int64_t)-2);
    CHECK(cpu.getRegister(riscvemu::Register::A4) ==
          (uint64_t)(int64_t)-8 >> 2);
    CHECK(cpu.getRegister(riscvemu::Register::A5) ==
          ((uint64_t)(int64_t)-8) >> 1);
}

TEST_CASE("testing addw instruction") {
    const auto* fp = "addw.bin";
    auto cpu       = setupTestContext(fp);
    cpu.run();

    CHECK(cpu.getRegister(riscvemu::Register::A2) == 0x7f00002a);
}

TEST_CASE("testing sub instruction") {
    const auto* fp = "sub.bin";
    auto cpu       = setupTestContext(fp);
    cpu.run();

    CHECK(cpu.getRegister(riscvemu::Register::A2) == 20);
}

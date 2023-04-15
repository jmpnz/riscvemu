#include "Machine.h"

#include <cstdint>
#include <cstdio>
#include <iostream>

#include "Instructions.h"

namespace riscvemu {

//=== MMU Methods Implementations ====//
// MMU stores data in byte aligned Little Endian format.
// e.g a single DWORD is laid out on byte alignment of four successive
// entries, to read a DWORD at a given address we first compute the actual
// address as [addr - BASE_ADDRESS] because the heap grows upwards.
// Each k-th read is shifted by k * 8 to lift the bits to their MSB position.
// After the shift the LSB's are zeroes, using OR we essentially replace
// the zeroed-out bits in the right hand side by the bits in the left hand side
// e.g our first read yields [11001100] our second read yields [01101110]
// after a left shift by eight on the second read we end up with :
//  [01101110 || 00000000] doing an OR with the previous read we end up with:
//  [11001100] OR [01101110 || 00000000] = [01101110 || 11001100]

/// @brief Load size number of bytes from memory at the given address.
/// @param addr Memory address.
/// @param size Read of the size (must be aligned to 8, 16, 32 ,64).
/// @return Memory contents at address cast as uint64_t.
// TODO: Typedef addr from uint64_t to avoid accidental swapping of arguments
// here. see lint: bugprone-easily-swappable-parameters
auto MMU::load(uint64_t addr, uint64_t size) // NOLINT
    -> uint64_t {
    switch (size) {
    case 8:
        return load8(addr);
    case 16:
        return load16(addr);
    case 32:
        return load32(addr);
    case 64:
        return load64(addr);
    default: {
        std::cerr << "Unaligned memory access error" << '\n';
        return -1;
    }
    }
}

/// @brief Store size number of bytes in memory at the given address.
/// @param addr Memory address.
/// @param size Read of the size (must be aligned to 8, 16, 32 ,64).
/// @param value Value to store.
/// @return void.
auto MMU::store(uint64_t addr, uint64_t size, uint64_t value) -> void { //NOLINT
    switch (size) {
    case 8:
        return store8(addr, value);
    case 16:
        return store16(addr, value);
    case 32:
        return store32(addr, value);
    case 64:
        return store64(addr, value);
    }
}

/// @brief Load a byte from memory.
/// @param addr
/// @return byte value represented as uint64_t.
auto MMU::load8(uint64_t addr) -> uint64_t {
    return (uint64_t)this->memory[addr - MemoryBaseAddr];
}

/// @brief Load a HALFWORD from memory.
/// @param addr
/// @return 2 byte value represented as uint64_t.
auto MMU::load16(uint64_t addr) -> uint64_t {
    return (uint64_t)this->memory[addr - MemoryBaseAddr] |
           (uint64_t)this->memory[addr - MemoryBaseAddr + 1] << 8;
}

/// @brief Load a WORD from memory.
/// @param addr
/// @return 4 byte value represented as uint64_t.
auto MMU::load32(uint64_t addr) -> uint64_t {
    return (uint64_t)this->memory[addr - MemoryBaseAddr] |
           (uint64_t)this->memory[addr - MemoryBaseAddr + 1] << 8 |
           (uint64_t)this->memory[addr - MemoryBaseAddr + 2] << 16 |
           (uint64_t)this->memory[addr - MemoryBaseAddr + 3] << 24;
}

/// @brief Load a DWORD from meory.
/// @param addr
/// @return 8 byte value represented as uint64_t.
auto MMU::load64(uint64_t addr) -> uint64_t {
    return (uint64_t)this->memory[addr - MemoryBaseAddr] |
           (uint64_t)this->memory[addr - MemoryBaseAddr + 1] << 8 |
           (uint64_t)this->memory[addr - MemoryBaseAddr + 2] << 16 |
           (uint64_t)this->memory[addr - MemoryBaseAddr + 3] << 24 |
           (uint64_t)this->memory[addr - MemoryBaseAddr + 4] << 32 |
           (uint64_t)this->memory[addr - MemoryBaseAddr + 5] << 40 |
           (uint64_t)this->memory[addr - MemoryBaseAddr + 6] << 48 |
           (uint64_t)this->memory[addr - MemoryBaseAddr + 7] << 56;
}

/// @brief Store a byte in memory.
/// @param addr
/// @param value
/// @return
auto MMU::store8(uint64_t addr, uint64_t value) -> void {
    this->memory[addr - MemoryBaseAddr] = (uint8_t)(value & 0xFF);
}

/// @brief Store a HALFWORD in memory.
/// @param addr
/// @param value
/// @return
auto MMU::store16(uint64_t addr, uint64_t value) -> void {
    this->memory[addr - MemoryBaseAddr]     = (uint8_t)(value & 0xFF);
    this->memory[addr - MemoryBaseAddr + 1] = (uint8_t)((value >> 8) & 0xFF);
}

/// @brief Store a WORD in memory.
/// @param addr
/// @param value
/// @return
auto MMU::store32(uint64_t addr, uint64_t value) -> void {
    this->memory[addr - MemoryBaseAddr]     = (uint8_t)(value & 0xFF);
    this->memory[addr - MemoryBaseAddr + 1] = (uint8_t)((value >> 8) & 0xFF);
    this->memory[addr - MemoryBaseAddr + 2] = (uint8_t)((value >> 16) & 0xFF);
    this->memory[addr - MemoryBaseAddr + 3] = (uint8_t)((value >> 24) & 0xFF);
}

/// @brief Store a DWORD in memory.
/// @param addr
/// @param value
/// @return
auto MMU::store64(uint64_t addr, uint64_t value) -> void {
    this->memory[addr - MemoryBaseAddr]     = (uint8_t)(value & 0xFF);
    this->memory[addr - MemoryBaseAddr + 1] = (uint8_t)((value >> 8) & 0xFF);
    this->memory[addr - MemoryBaseAddr + 2] = (uint8_t)((value >> 16) & 0xFF);
    this->memory[addr - MemoryBaseAddr + 3] = (uint8_t)((value >> 24) & 0xFF);
    this->memory[addr - MemoryBaseAddr + 4] = (uint8_t)((value >> 32) & 0xFF);
    this->memory[addr - MemoryBaseAddr + 5] = (uint8_t)((value >> 40) & 0xFF);
    this->memory[addr - MemoryBaseAddr + 6] = (uint8_t)((value >> 48) & 0xFF);
    this->memory[addr - MemoryBaseAddr + 7] = (uint8_t)((value >> 56) & 0xFF);
}

/// @brief Dump memory contents to stdout.
/// @param
/// @return
auto MMU::dumpMemory() -> void {
    for (size_t i = MemoryBaseAddr; i > 0; --i) {
        printf("Memory[%zu] = %x\n", i, this->memory[i]);
    }

    printf("\n");
}

//==== CPU Methods Implementations ====//

/// @brief Fetches the next instruction to execute stored @ pc.
/// @return Returns the instruction in 32-bit format (encoded.)
auto CPU::fetch() -> uint32_t { return this->ctx->mmu.load(this->pc, 32); }

/// @brief Decode the fetched instruction to execute.
auto CPU::decode(uint32_t instruction) -> Instruction {
    // TODO: ideally we want instruction to hold a union
    // of instruction types.
    return Instruction{.opcode      = instruction & 0b1111111,
                       .instruction = instruction};
}

auto CPU::setRegister(Register reg, uint64_t value) -> void {
    if (reg != Register::Zero) {
        this->registers[(int)reg] = value;
    }
}

auto CPU::dumpRegisters() -> void {
    for (int i = 0; i < 32; i++) {
        printf("x[%d] = %llu\n", i, this->registers[i]);
    }
    printf("\n");
}

auto CPU::execute(const Instruction& instruction) -> void {
    printf("execute == OPCode : %x\n", instruction.opcode);
    switch (instruction.opcode) {
    case 0b0000011: {
        // Itype instruction.
        auto inst = Itype(instruction.instruction);
        auto rs1  = this->registers[(size_t)inst.Rs1];
        auto imm  = inst.Imm;

        if (inst.Funct3 == 0b000) {
            // LB : load byte at the effective address [rs1] +  Imm into Rd.
            auto addr  = rs1 + imm;
            auto value = (int64_t)this->ctx->mmu.load(addr, 8);
            this->setRegister(inst.Rd, value);
        } else if (inst.Funct3 == 0b001) {
            // LH: load halfword at the effective address [rs1] + Imm into Rd.
            auto addr  = rs1 + imm;
            auto value = (int64_t)this->ctx->mmu.load(addr, 16);
            this->setRegister(inst.Rd, value);
        } else if (inst.Funct3 == 0b010) {
            // LW: load word at the effective address [rs1] + Imm into Rd.
            auto addr  = rs1 + imm;
            auto value = (int64_t)this->ctx->mmu.load(addr, 32);
            this->setRegister(inst.Rd, value);
        } else if (inst.Funct3 == 0b100) {
            // LBU: load byte upper at the effective address [rs1] + Imm into Rd.
            auto addr  = rs1 + imm;
            auto value = (uint64_t)this->ctx->mmu.load(addr, 8);
            this->setRegister(inst.Rd, value);
        } else if (inst.Funct3 == 0b101) {
            auto addr  = rs1 + imm;
            auto value = (uint64_t)this->ctx->mmu.load(addr, 16);
            this->setRegister(inst.Rd, value);
        }
        break;
    }
    case 0b0110011: {
        auto inst    = Rtype(instruction.instruction);
        uint64_t rs1 = this->registers[(size_t)inst.Rs1];
        uint64_t rs2 = this->registers[(size_t)inst.Rs2];

        if (inst.Funct3 == 0b000 && inst.Funct7 == 0b0000000) {
            // ADD instruction
            this->setRegister(inst.Rd, rs1 + rs2);
        }
        break;
    }
    case 0b0010011: {
        auto inst = Itype(instruction.instruction);
        auto rs1  = this->registers[(size_t)inst.Rs1];
        auto imm  = inst.Imm;

        if (inst.Funct3 == 0b000) {
            this->setRegister(inst.Rd, rs1 + imm);
        }
        break;
    }

    default: {
        printf("Unknown instruction : %x\n", instruction.instruction);
        break;
    }
    }
}

void CPU::run() {
    this->registers[10] = 0xffff;
    while (this->pc < (MemoryBaseAddr + 0xc)) {
        auto nextInst = this->fetch();
        printf("Next instruction: %x\n", nextInst);
        auto inst = this->decode(nextInst);
        printf("Opcode: %x | Instruction : %x\n", inst.opcode,
               inst.instruction);
        printf("Program counter : %zx\n", this->pc);
        this->pc += 4;
        this->execute(inst);

        printf("Program counter : %zx\n", this->pc);
        if (this->pc == 0 || inst.instruction == 1)
            break;
    }
}

} // namespace riscvemu

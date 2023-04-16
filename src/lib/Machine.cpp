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
auto MMU::load(VirtualAddress addr, size_t size) // NOLINT
    -> uint64_t {
    // If the memory access is not within the access range
    // throw a LoadAccessFault exception.
    if (!withinRange(addr)) {
        throw LoadAccessFault();
    }
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
auto MMU::store(VirtualAddress addr, size_t size, uint64_t value)
    -> void { // NOLINT
    assert(addr >= MemoryBaseAddr && addr <= MemoryEndAddr);
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
auto MMU::load8(VirtualAddress addr) -> uint64_t {
    printf("Address to access : %llx\n", addr);
    printf("Memory address access : %llx\n", addr - MemoryBaseAddr);
    return (uint64_t)this->memory[addr - MemoryBaseAddr];
}

/// @brief Load a HALFWORD from memory.
/// @param addr
/// @return 2 byte value represented as uint64_t.
auto MMU::load16(VirtualAddress addr) -> uint64_t {
    return (uint64_t)this->memory[addr - MemoryBaseAddr] |
           (uint64_t)this->memory[addr - MemoryBaseAddr + 1] << 8;
}

/// @brief Load a WORD from memory.
/// @param addr
/// @return 4 byte value represented as uint64_t.
auto MMU::load32(VirtualAddress addr) -> uint64_t {
    return (uint64_t)this->memory[addr - MemoryBaseAddr] |
           (uint64_t)this->memory[addr - MemoryBaseAddr + 1] << 8 |
           (uint64_t)this->memory[addr - MemoryBaseAddr + 2] << 16 |
           (uint64_t)this->memory[addr - MemoryBaseAddr + 3] << 24;
}

/// @brief Load a DWORD from meory.
/// @param addr
/// @return 8 byte value represented as uint64_t.
auto MMU::load64(VirtualAddress addr) -> uint64_t {
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
auto MMU::store8(VirtualAddress addr, uint64_t value) -> void {
    this->memory[addr - MemoryBaseAddr] = (uint8_t)(value & 0xFF);
}

/// @brief Store a HALFWORD in memory.
/// @param addr
/// @param value
/// @return
auto MMU::store16(VirtualAddress addr, uint64_t value) -> void {
    this->memory[addr - MemoryBaseAddr]     = (uint8_t)(value & 0xFF);
    this->memory[addr - MemoryBaseAddr + 1] = (uint8_t)((value >> 8) & 0xFF);
}

/// @brief Store a WORD in memory.
/// @param addr
/// @param value
/// @return
auto MMU::store32(VirtualAddress addr, uint64_t value) -> void {
    this->memory[addr - MemoryBaseAddr]     = (uint8_t)(value & 0xFF);
    this->memory[addr - MemoryBaseAddr + 1] = (uint8_t)((value >> 8) & 0xFF);
    this->memory[addr - MemoryBaseAddr + 2] = (uint8_t)((value >> 16) & 0xFF);
    this->memory[addr - MemoryBaseAddr + 3] = (uint8_t)((value >> 24) & 0xFF);
}

/// @brief Store a DWORD in memory.
/// @param addr
/// @param value
/// @return
auto MMU::store64(VirtualAddress addr, uint64_t value) -> void {
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

/// @brief Set register reg with given value.
/// @param reg
/// @param value
/// @return void
auto CPU::setRegister(Register reg, uint64_t value) -> void {
    if (reg != Register::Zero) {
        this->registers[(size_t)reg] = value;
    }
}

/// @brief Load contents at addr.
/// @param addr
/// @param size
/// @return Value upcast to uint64_t.
auto CPU::load(VirtualAddress addr, size_t size) -> uint64_t {
    return this->ctx->mmu.load(addr, size);
}

/// @brief Store contents at addr.
/// @param addr
/// @param size
/// @param value
/// @return void
auto CPU::store(VirtualAddress addr, size_t size, uint64_t value) -> void {
    return this->ctx->mmu.store(addr, size, value);
}

auto CPU::dumpRegisters() -> void {
    for (int i = 0; i < 32; i++) {
        Register const reg = getRegisterFromIndex(i);
        printf("x[%d]/%s  =  0x%llx\n", i, getRegisterABIName(reg),
               this->registers[i]);
    }
    printf("Program Counter: 0x%zx\n", this->pc);
    printf("\n");
}

auto CPU::execute(const Instruction& instruction) -> void {
    switch (instruction.opcode) {
    case 0b0110111: {
        // Utype instruction (LUI).
        auto inst = Utype(instruction.instruction);
        auto rd   = inst.Rd;
        auto imm  = inst.Imm;
        // LUI: load upper immediate places the the immediate value
        // in the top 20 bits of the destination register rd filling
        // the lowest 12 bits with zeroes.
        auto value = (uint64_t)(int64_t)(imm & 0xFFFFF000);
        this->setRegister(rd, value);
        break;
    }
    case 0b0010111: {
        // Utype instruction (AUIPC).
        auto inst = Utype(instruction.instruction);
        auto rd   = inst.Rd;
        auto imm  = inst.Imm;
        printf("Instruction bits :%x\n", instruction.instruction);
        // AUIPC: add upper immediate to pc builds a pc-relative address.
        auto offset = ((int64_t)this->pc + (int64_t)imm) - 4;
        this->setRegister(rd, offset);
        break;
    }
    case 0b1101111: {
        // Jtype instrtuction (JAL)
        auto inst = Jtype(instruction.instruction);
        auto rd   = inst.Rd;
        auto imm  = inst.Imm;
        // JAL: jump and link
        this->setRegister(rd, this->pc);
        this->pc = this->pc + (int64_t)imm - 4;
        // TODO: raise Misaligned exception if address is misaligned
        break;
    }
    case 0b1100111: {
        // Itype instruction (JALR)
        auto inst = Itype(instruction.instruction);
        auto rd   = inst.Rd;
        auto rs1  = inst.Rs1;
        auto imm  = inst.Imm;
        // JALR: (indirect) jump and link register.
        auto oldPC = this->pc;
        this->pc   = (this->registers[(size_t)rs1] + (int64_t)imm) & 0xFFFFFFFE;
        this->setRegister(rd, oldPC);
        // TODO: raise Misaligned exception if address is misaligned
        break;
    }
    case 0b0000011: {
        // Itype instruction.
        auto inst = Itype(instruction.instruction);
        auto rs1  = this->registers[(size_t)inst.Rs1];
        auto imm  = (int64_t)inst.Imm;

        if (inst.Funct3 == 0b000) {
            // LB : load byte at the effective address [rs1] +  Imm and sign into Rd.
            auto addr  = (uint64_t)rs1 + (uint64_t)imm;
            auto value = (int64_t)((int8_t)this->load(addr, 8));
            this->setRegister(inst.Rd, value);
        } else if (inst.Funct3 == 0b001) {
            // LH: load halfword at the effective address [rs1] + Imm and sign extend
            // into Rd.
            auto addr  = rs1 + imm;
            auto value = (int64_t)((int16_t)this->load(addr, 16));
            this->setRegister(inst.Rd, value);
        } else if (inst.Funct3 == 0b010) {
            // LW: load word at the effective address [rs1] + Imm and sign extend
            // into Rd.
            auto addr  = rs1 + imm;
            auto value = (int64_t)((int32_t)this->load(addr, 32));
            this->setRegister(inst.Rd, value);
        } else if (inst.Funct3 == 0b011) {
            // LD: Load double word unsigned at the effective address [rs1] + Imm
            // into Rd.
            auto addr  = rs1 + imm;
            auto value = (int64_t)this->load(addr, 64);
            this->setRegister(inst.Rd, value);
        } else if (inst.Funct3 == 0b100) {
            // LBU: load byte unsigned at the effective address [rs1] + Imm into Rd.
            auto addr  = rs1 + imm;
            auto value = this->load(addr, 8);
            this->setRegister(inst.Rd, value);
        } else if (inst.Funct3 == 0b101) {
            // LHU (RV64i): load half word unsigned at the effective address
            // [rs1] + Imm into Rd.
            auto addr  = rs1 + imm;
            auto value = this->load(addr, 16);
            this->setRegister(inst.Rd, value);
        } else if (inst.Funct3 == 0b110) {
            // LWU: load word unsigned at the effective address [rs1] + Imm into Rd.
            auto addr  = rs1 + imm;
            auto value = this->load(addr, 32);
            this->setRegister(inst.Rd, value);
        }
        break;
    }
    case 0b0100011: {
        // Stype instruction.
        auto inst  = Stype(instruction.instruction);
        auto imm   = inst.Imm;
        auto rs1   = inst.Rs1;
        auto rs2   = inst.Rs2;
        auto addr  = this->registers[(size_t)rs1] + (uint64_t)imm;
        auto value = this->registers[(size_t)rs2];

        if (inst.Funct3 == 0b000) {
            // SB: Store byte at address.
            printf("Store @ %llx : %llu", this->registers[(size_t)rs2], addr);
            this->store(addr, 8, value);
        }
        if (inst.Funct3 == 0b001) {
            // SH: Store half word at address.
            printf("Store @ %llx : %llu", this->registers[(size_t)rs2], addr);
            this->store(addr, 16, value);
            printf("Memory @ %llx : %llu", addr, this->ctx->mmu.load16(addr));
        }
        if (inst.Funct3 == 0b010) {
            // SW: Store word at address.
            printf("Store value %llx @ %llx", this->registers[(size_t)rs2],
                   addr - MemoryBaseAddr);
            this->store(addr, 32, value);
            printf("Memory @ %llx : %llu", addr, this->ctx->mmu.load32(addr));
        }
        if (inst.Funct3 == 0b011) {
            // SD: Store double word at address.
            printf("Store @ %llx : %llu", this->registers[(size_t)rs2], addr);
            this->store(addr, 64, value);
        }
        break;
    }
    case 0b0110011: {
        auto inst = Rtype(instruction.instruction);
        auto rs1  = this->registers[(size_t)inst.Rs1];
        auto rs2  = this->registers[(size_t)inst.Rs2];

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
            // ADDI: add immmediate value to rs1 store result in rd.
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
    auto count = 0;
    while (true) {
        count += 1;
        if (this->pc < MemoryBaseAddr ||
            (MemoryBaseAddr + this->ctx->code.size()) <= this->pc) {
            break;
        }
        if (count > 10000) {
            break;
        }
        auto nextInst = this->fetch();
        auto inst     = this->decode(nextInst);
        this->pc += 4;
        printf("==== PRE-EXECUTE ====\n");
        this->dumpRegisters();
        this->execute(inst);
        printf("==== POST-EXECUTE ====\n");
        this->dumpRegisters();

        if (this->pc == 0 || inst.instruction == 1)
            break;
    }
}

} // namespace riscvemu

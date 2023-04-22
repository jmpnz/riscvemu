
#include <cstdint>
#include <cstdio>
#include <iostream>

#include "CSR.h"
#include "Decoder.h"
#include "Instructions.h"
#include "Machine.h"

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
    if (!withinRange(addr)) {
        throw LoadAccessFault();
    }
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
    return Instruction{.opcode      = OPCode(instruction & 0b1111111),
                       .instruction = instruction};
}

/// @brief Get value stored in the register.
/// @param reg: Register
/// @return uint64_t
auto CPU::getRegister(Register reg) -> uint64_t {
    return this->registers[(size_t)reg];
}

/// @brief Get value stored in the control or status registers.
/// @param addr uitn64_t
/// @return uint64_t
auto CPU::getCSR(uint64_t addr) -> uint64_t { return this->csrs.csrs[addr]; }

/// @brief Set register reg with given value.
/// @param reg: Register
/// @param value uint64_t
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
    printf("\n");
}

/// @brief Execute an instruction.
/// @param Instruction&
/// @return void
auto CPU::execute(const Instruction& instruction) -> void {
    switch (instruction.opcode) {
    case OPCode::LUI: {
        // Utype instruction (LUI).
        auto inst = Utype(instruction.instruction);
        auto rd   = inst.Rd;
        auto imm  = inst.Imm;
        // LUI: load upper immediate places the the immediate value
        // in the top 20 bits of the destination register rd filling
        // the lowest 12 bits with zeroes.
        auto value = signExtend(imm & 0xFFFFF000, 64);
        this->setRegister(rd, value);
        break;
    }
    case OPCode::AUIPC: {
        // Utype instruction (AUIPC).
        auto inst = Utype(instruction.instruction);
        auto rd   = inst.Rd;
        auto imm  = inst.Imm;
        // AUIPC: add upper immediate to pc builds a pc-relative address.
        auto offset = (this->pc + signExtend(imm & 0xFFFFF000, 64) - 4);
        this->setRegister(rd, offset);
        break;
    }
    case OPCode::JAL: {
        // Jtype instrtuction (JAL)
        auto inst = Jtype(instruction.instruction);
        auto rd   = inst.Rd;
        auto imm  = inst.Imm;
        // JAL: jump and link
        this->setRegister(rd, this->pc);
        this->pc = this->pc + signExtend(imm - 4, 64);
        // TODO: raise Misaligned exception if address is misaligned
        break;
    }
    case OPCode::JALR: {
        // Itype instruction (JALR)
        auto inst = Itype(instruction.instruction);
        auto rd   = inst.Rd;
        auto rs1  = inst.Rs1;
        auto imm  = inst.Imm;
        // JALR: (indirect) jump and link register.
        auto oldPC = this->pc;
        this->pc   = (this->registers[(size_t)rs1] + signExtend(imm, 64)) &
                   0xFFFFFFFE - 4;
        this->setRegister(rd, oldPC);
        // TODO: raise Misaligned exception if address is misaligned
        break;
    }
    case OPCode::BRANCH: {
        // Btype instruction (Branching).
        auto inst = Btype(instruction.instruction);
        auto imm  = inst.Imm;
        auto rs1  = inst.Rs1;
        auto rs2  = inst.Rs2;

        if (inst.Funct3 == 0b000) /* BEQ */ {
            // BEQ : take the branch if [rs1] == [rs2]
            if (this->getRegister(rs1) == this->getRegister(rs2)) {
                this->pc = this->pc + signExtend(imm, 64);
            }
        }
        if (inst.Funct3 == 0b001) /* BNE */ {
            // BNE : take the branch if [rs1] != [rs2]
            if (this->getRegister(rs1) != this->getRegister(rs2)) {
                this->pc = (this->pc + signExtend(imm, 64));
            }
        }
        if (inst.Funct3 == 0b100) /* BLT */ {
            // BLT : take the branch if [rs1] < [rs2]
            if ((int64_t)this->getRegister(rs1) <
                (int64_t)this->getRegister(rs2)) {
                this->pc = this->pc + (int64_t)imm;
            }
        }
        if (inst.Funct3 == 0b101) /* BGE */ {
            // BGE : take the branch if [rs1] >= [rs2]
            if ((int64_t)this->getRegister(rs1) >=
                (int64_t)this->getRegister(rs2)) {
                this->pc = this->pc + (int64_t)imm;
            }
        }
        if (inst.Funct3 == 0b110) /* BLTU */ {
            // BLTU : (unsigned) take the branch if [rs1] < [rs2]
            if (this->getRegister(rs1) < this->getRegister(rs2)) {
                this->pc = this->pc + (int64_t)imm;
            }
        }
        if (inst.Funct3 == 0b111) /* BGEU */ {
            // BGEU : (unsigned) take the branch if [rs1] >= [rs2]
            if (this->getRegister(rs1) >= this->getRegister(rs2)) {
                this->pc = this->pc + (int64_t)imm;
            }
        }

        break;
    }
    case OPCode::LOAD: {
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
    case OPCode::STORE: {
        // Stype instruction.
        auto inst  = Stype(instruction.instruction);
        auto imm   = inst.Imm;
        auto rs1   = inst.Rs1;
        auto rs2   = inst.Rs2;
        auto addr  = this->getRegister(rs1) + (int64_t)imm;
        auto value = this->getRegister(rs2);

        if (inst.Funct3 == 0b000) {
            // SB: Store byte at address.
            this->store(addr, 8, value);
        }
        if (inst.Funct3 == 0b001) {
            // SH: Store half word at address.
            this->store(addr, 16, value);
        }
        if (inst.Funct3 == 0b010) {
            // SW: Store word at address.
            this->store(addr, 32, value);
        }
        if (inst.Funct3 == 0b011) {
            // SD: Store double word at address.
            this->store(addr, 64, value);
        }
        break;
    }
    case OPCode::ARITHR: {
        auto inst = Rtype(instruction.instruction);
        auto rs1  = this->getRegister(inst.Rs1);
        auto rs2  = this->getRegister(inst.Rs2);

        if (inst.Funct3 == 0b000 && inst.Funct7 == 0b0000000) {
            // ADD: add [rs1] to [rs2] store the result in [rd].
            this->setRegister(inst.Rd, rs1 + rs2);
        }
        if (inst.Funct3 == 0b000 && inst.Funct7 == 0b0000010) {
            // SUB: substract [rs2] from [rs1] store the result in [rd].
            this->setRegister(inst.Rd, rs1 - rs2);
        }
        if (inst.Funct3 == 0b001 && inst.Funct7 == 0b0000000) {
            // SLL: Shift Left Logical.
            // auto shamt = (this->getRegister(inst.Rs2) & 0x3f);
            auto value = rs1 << rs2;
            this->setRegister(inst.Rd, value);
        }
        if (inst.Funct3 == 0b010 && inst.Funct7 == 0b0000000) {
            // SLT: Set if Less Than
            auto value = ((int64_t)rs1 < (int64_t)rs2) ? 1 : 0;
            this->setRegister(inst.Rd, value);
        }
        if (inst.Funct3 == 0b011 && inst.Funct7 == 0b0000000) {
            // SLTU: Set if Less Than Unsigned.
            auto value = (rs1 < rs2) ? 1 : 0;
            this->setRegister(inst.Rd, value);
        }
        if (inst.Funct3 == 0b100 && inst.Funct7 == 0b0000000) {
            // XOR: Set [rd] to [rs1] ^ [rs2].
            auto value = rs1 ^ rs2;
            this->setRegister(inst.Rd, value);
        }
        if (inst.Funct3 == 0b101 && inst.Funct7 == 0b0000000) {
            // SRL: Shift Right Logical.
            auto shamt = (this->getRegister(inst.Rs2) & 0x3f);
            auto value = rs1 >> shamt;
            this->setRegister(inst.Rd, value);
        }
        if (inst.Funct3 == 0b101 && inst.Funct7 == 0b0100000) {
            // SRA: Shift Right Arithmetic.
            auto shamt = rs2 & 0x3f;
            auto value = (uint64_t)((int64_t)rs1 >> shamt);
            this->setRegister(inst.Rd, value);
        }
        if (inst.Funct3 == 0b110 && inst.Funct7 == 0b0000000) {
            // OR: Set [rd] to [r1] | [r2].
            auto value = rs1 | rs2;
            this->setRegister(inst.Rd, value);
        }
        if (inst.Funct3 == 0b111 && inst.Funct7 == 0b0000000) {
            // AND: Set [rd] to [r1] ^ [r2].
            auto value = rs1 & rs2;
            this->setRegister(inst.Rd, value);
        }
        break;
    }
    case OPCode::ARITHI: {
        auto inst = Itype(instruction.instruction);
        auto rs1  = this->registers[(size_t)inst.Rs1];
        auto imm  = inst.Imm;

        if (inst.Funct3 == 0b000) {
            // ADDI: add immmediate value to rs1 store result in rd.
            this->setRegister(inst.Rd, rs1 + imm);
        }
        if (inst.Funct3 == 0b010) {
            // SLTI : Set if Less Than Immediate.
            auto value = (this->getRegister(inst.Rs1) < (int64_t)imm) ? 1 : 0;
            this->setRegister(inst.Rd, value);
        }
        if (inst.Funct3 == 0b011) {
            // SLTIU: Set if Less Than Immediate (Unsigned).
            auto value = (this->getRegister(inst.Rs1) < imm) ? 1 : 0;
            this->setRegister(inst.Rd, value);
        }
        if (inst.Funct3 == 0b100) {
            // XORI: Compute bitwise exclusive-OR of the sign-extended
            // immediate and [rs1], writing the result to [rd].
            auto value = (this->getRegister(inst.Rs1) ^ imm);
            this->setRegister(inst.Rd, value);
        }
        if (inst.Funct3 == 0b110) {
            // ORI: Compute bitwise OR of the sign-extended
            // immediate and [rs1], writing the result to [rd].
            auto value = (this->getRegister(inst.Rs1) | imm);
            this->setRegister(inst.Rd, value);
        }
        if (inst.Funct3 == 0b111) {
            // AND: Compute bitwise AND of the sign-extended
            // immediate and [rs1], writing the result to [rd].
            auto value = (this->getRegister(inst.Rs1) & imm);
            this->setRegister(inst.Rd, value);
        }
        if (inst.Funct3 == 0b001) {
            // SLLI: Shift Left Logical Immediate
            auto shamt = (uint32_t)(imm & 0x3f);
            this->setRegister(inst.Rd, this->getRegister(inst.Rs1) << shamt);
        }
        if (inst.Funct3 == 0b101) {
            auto funct7 = Rtype(instruction.instruction).Funct7;
            auto shamt  = (uint32_t)(imm & 0x3f);
            if ((funct7) == 0x00) {
                // SRLI: Shift Right Logical Immediate.
                this->setRegister(inst.Rd,
                                  this->getRegister(inst.Rs1) >> shamt);

            } else if ((funct7 >> 1) == 0x10) {
                // SRAI: Shift Right Arithmetic Immediate.
                this->setRegister(
                    inst.Rd,
                    (uint64_t)((int64_t)this->getRegister(inst.Rs1) >> shamt));
            }
        }
        break;
    }
    case OPCode::ARITHIW: {
        // Decode as Itype for ADDI
        auto instItype = Itype(instruction.instruction);
        auto imm       = instItype.Imm;
        auto rd        = instItype.Rd;
        auto rs1       = instItype.Rs1;
        // Decode as Rtype for SLLIW/SRLIW/SRAIW
        auto inst   = Rtype(instruction.instruction);
        auto funct7 = inst.Funct7;
        auto funct3 = instItype.Funct3;
        if (funct3 == 0b000) {
            // ADDIW
            this->setRegister(
                rd, (int64_t)(int32_t)(this->getRegister(rs1) + (int64_t)imm));
        }
        if (funct3 == 0b001) {
            // SLLIW
            auto shamt = (uint32_t)(imm & 0x3f);
            this->setRegister(inst.Rd,
                              (int64_t)(int32_t)this->getRegister(inst.Rs1)
                                  << shamt);
        }
        if (funct3 == 0b101) {
            auto shamt = (uint32_t)(imm & 0x3f);
            if (funct7 >> 1 == 0x00) {
                // SRLIW
                this->setRegister(
                    inst.Rd, (int64_t)(int32_t)this->getRegister(rs1) >> shamt);
            }
            if (funct7 >> 1 == 0x10) {
                // SRAIW
                this->setRegister(
                    inst.Rd, (int64_t)(int32_t)this->getRegister(rs1) >> shamt);
            }
        }
    }
    case OPCode::ARITHRW: {
        // Stype instruction.
        auto inst   = Rtype(instruction.instruction);
        auto rd     = inst.Rd;
        auto rs1    = inst.Rs1;
        auto rs2    = inst.Rs2;
        auto funct3 = inst.Funct3;
        auto funct7 = inst.Funct7;
        if (funct3 == 0b000) {
            if (funct7 >> 1 == 0x00) {
                // ADDW
                auto value = (int64_t)(int32_t)this->getRegister(rs1) +
                             (int64_t)this->getRegister(rs2);
                this->setRegister(rd, value);
            }
            if (funct7 >> 1 == 0x10) {
                // SUBW
                auto value = (int64_t)(int32_t)this->getRegister(rs1) -
                             (int64_t)this->getRegister(rs2);
                this->setRegister(rd, value);
            }
        }
        if (funct3 == 0b001) {
            // SLLW
            auto value = (int64_t)(int32_t)(this->getRegister(rs1)
                                            << this->getRegister(rs2));
            this->setRegister(rd, value);
        }
        if (funct3 == 0b101) {
            if (funct7 >> 1 == 0x00) {
                // SRLW
                auto value = (int64_t)(int32_t)(this->getRegister(rs1) >>
                                                this->getRegister(rs2));
                this->setRegister(rd, value);
            }
            if (funct7 >> 1 == 0x10) {
                // SRAW
                auto value =
                    (int64_t)(int32_t)(this->getRegister(rs1) >>
                                       (int32_t)this->getRegister(rs2));
                this->setRegister(rd, value);
            }
        }
    }

    case OPCode::CSR: {
        auto inst   = instruction.instruction;
        auto rd     = (uint64_t)((inst & 0x00000f80) >> 7);
        auto rs1    = (uint64_t)((inst & 0x000f8000) >> 15);
        auto addr   = (uint64_t)((inst & 0xfff00000) >> 20);
        auto funct3 = (inst & 0x00007000) >> 12;
        if (funct3 == 0x1) {
            // CSRRW
            auto t = this->csrs.load(addr);
            this->csrs.store(addr, this->registers[rs1]);
            this->registers[rd] = t;
        }
        if (funct3 == 0x2) {
            // CSRRS
            auto t = this->csrs.load(addr);
            this->csrs.store(addr, t | this->registers[rs1]);
            this->registers[rd] = t;
        }
        if (funct3 == 0x3) {
            // CSRRC
            auto t = this->csrs.load(addr);
            this->csrs.store(addr, t & (~this->registers[rs1])); //NOLINT
            this->registers[rd] = t;
        }
        if (funct3 == 0x5) {
            // CSRRWI
            auto zimm           = rs1;
            this->registers[rd] = this->csrs.load(addr);
            this->csrs.store(addr, zimm);
        }
        if (funct3 == 0x6) {
            // CSRRSI
            auto zimm = rs1;
            auto t    = this->csrs.load(addr);
            this->csrs.store(addr, t | zimm);
            this->registers[rd] = t;
        }
        if (funct3 == 0x7) {
            // CSRRCI
            auto zimm = rs1;
            auto t    = this->csrs.load(addr);
            this->csrs.store(addr, t & (~zimm)); //NOLINT
            this->registers[rd] = t;
        }
        // Control and Status instructions.
        //
        // ECALL & EBREAK differ on their Funct7 but since we don't need the debug
        // calls we can just skip them.

        break;
    }

    default: {
        throw IllegalInstruction();
        break;
    }
    }
}

void CPU::run() {
    while (true) {
        if (this->pc < MemoryBaseAddr ||
            (MemoryBaseAddr + this->ctx->code.size()) <= this->pc) {
            break;
        }
        auto nextInst = this->fetch();
        auto inst     = this->decode(nextInst);
        this->pc += 4;
        try {
            this->execute(inst);
        } catch (riscvemu::IllegalInstruction& e) {
            printf("Exception Raised: Illegal Instruction: %s\n", e.what());
            break;
        } catch (riscvemu::LoadAccessFault& e) {
            printf("Exception Raised: Load Access Fault: %s\n", e.what());
            break;
        }
    }
}

} // namespace riscvemu

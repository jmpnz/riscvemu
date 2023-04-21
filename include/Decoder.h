#ifndef DECODER_H
#define DECODER_H

#include "Instructions.h"
#include <cstdint>

namespace riscvemu {

// Mask for decoding opcode from instruction bits, opcodes are stored
// in the first 7 bits.
static constexpr uint32_t OPCodeMask = 0x7f;

// Sign extend bits in the lower part of value into signed int64_t.
static inline auto signExtend(uint64_t value, uint64_t bits) -> int64_t {
    return ((int64_t)(value << (64 - bits))) >> (64 - bits);
}

/// @brief Rtype instructions for register to register operations.
struct Rtype {
    uint32_t Funct7;
    Register Rs2;
    Register Rs1;
    uint32_t Funct3;
    Register Rd;

    /// @brief Construction takes an encoded instruction
    /// and unpacks the components.
    Rtype(uint32_t inst) {
        Funct7 = (inst >> 25) & 0b1111111;
        Rs2    = Register((inst >> 20) & 0b11111);
        Rs1    = Register((inst >> 15) & 0b11111);
        Funct3 = (inst >> 12) & 0b111;
        Rd     = Register((inst >> 7) & 0b11111);
    }
};

/// @brief Itype instructions for short immediates and loads.
struct Itype {
    int32_t Imm;
    Register Rs1;
    Register Rd;
    uint32_t Funct3;

    /// @brief Construction takes an encoded instruction
    /// and unpacks the components.
    Itype(uint32_t inst) {
        Imm    = ((int32_t)(inst & 0xfff00000)) >> 20;
        Rs1    = Register((inst >> 15) & 0b11111);
        Rd     = Register((inst >> 7) & 0b11111);
        Funct3 = (inst >> 12) & 0b111;
    }
};

/// @brief Stype instructions for stores.
struct Stype {
    int32_t Imm;
    Register Rs2;
    Register Rs1;
    uint32_t Funct3;

    /// @brief Construction takes an encoded instruction
    /// and unpacks the component.
    Stype(uint32_t inst) {
        uint32_t const imm40  = (inst >> 7) & 0b11111;
        uint32_t const imm115 = (inst >> 25) & 0b1111111;
        uint32_t const imm    = (imm115 << 5) | imm40;
        Imm                   = ((int32_t)imm << 20) >> 20;
        Rs2                   = Register((inst >> 20) & 0b11111);
        Rs1                   = Register((inst >> 15) & 0b11111);
        Funct3                = (inst >> 12) & 0b111;
    }
};

/// @brief Jtype instructions for unconditional jumps.
struct Jtype {
    Register Rd;
    int32_t Imm;

    /// @brief Construction takes an encoded instruction
    /// and unpacks the components.
    Jtype(uint32_t inst) {
        uint32_t const imm20   = (inst >> 31) & 1;
        uint32_t const imm101  = (inst >> 21) & 0b1111111111;
        uint32_t const imm11   = (inst >> 20) & 1;
        uint32_t const imm1912 = (inst >> 12) & 0b11111111;
        uint32_t const imm =
            (imm20 << 20) | (imm1912 << 12) | (imm11 << 11) | (imm101 << 1);

        Imm = ((int32_t)imm << 11) >> 11;
        Rd  = Register((inst >> 7) & 0b11111);
    }
};

/// @brief Btype instructions for conditional branches.
struct Btype {
    int32_t Imm;
    Register Rs2;
    Register Rs1;
    uint32_t Funct3;

    /// @brief Construction takes an encoded instruction
    /// and unpacks the components.
    Btype(uint32_t inst) {
        uint32_t const imm12  = (inst >> 31) & 1;
        uint32_t const imm105 = (inst >> 25) & 0b111111;
        uint32_t const imm41  = (inst >> 8) & 0b1111;
        uint32_t const imm11  = (inst >> 7) & 1;

        uint32_t const imm =
            (imm12 << 12) | (imm11 << 11) | (imm105 << 5) | (imm41 << 1);

        Imm    = (((int32_t)imm) << 19) >> 19;
        Rs2    = Register((inst >> 20) & 0b11111);
        Rs1    = Register((inst >> 15) & 0b11111);
        Funct3 = (inst >> 12) & 0b111;
    }
};

/// @brief Utype instructions for long immediates.
struct Utype {
    int32_t Imm;
    Register Rd;

    Utype(uint32_t inst) {
        Imm = (int32_t)(inst & 0xfffff000);
        Rd  = Register((inst >> 7) & 0b11111);
    }
};

//Decode an instruction given its type.
template <typename T> auto decodeInstruction(const uint32_t instruction) -> T {
    return T(instruction);
}
}; // namespace riscvemu

#endif

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <cassert>
#include <cstdint>
#include <string>

namespace riscvemu {

/// @brief Register enum encodes the ABI name for RISC-V registers.
/// The ABI names are described in the RISC-V calling conventions.
enum class Register {
    Zero, // Hard-wired zero
    Ra,   // Return address
    Sp,   // x2 is used as the stack pointer
    Gp,   // Global pointer
    Tp,   // Thread pointer
    T0,   // Temporary
    T1,   // Temporary T1
    T2,   // Temporary T2
    S0,   // Saved register (frame pointer)
    S1,   // Saved register
    A0,   // Function arguments / Return values
    A1,   // Function arguments
    A2,   // Function arguments
    A3,   // Function arguments
    A4,   // Function arguments
    A5,   // Function arguments
    A6,   // Function arguments
    A7,   // Function arguments
    S2,   // Saved registers
    S3,   // Saved registers
    S4,   // Saved registers
    S5,   // Saved registers
    S6,   // Saved registers
    S7,   // Saved registers
    S8,   // Saved registers
    S9,   // Saved registers
    S10,  // Saved registers
    S11,  // Saved registers
    T3,   // Temporary T3
    T4,   // Temporary T4
    T5,   // Temporary T5
    T6,   // Temporary T6
    Pc,   // Program counter  (not explicitely declared)
    Unknown,
};

/// @brief Return typed Register value from register index.
/// @param idx
/// @return Register (ABI) corresponding to index.
auto getRegisterFromIndex(uint64_t idx) -> Register;

/// @brief Returns ABI name of register as a string
/// @param r
/// @return const char*
auto getRegisterABIName(Register r) -> const char*;

/// @brief Rtype instructions.
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

/// @brief Itype instructions.
struct Itype {
    int32_t Imm;
    Register Rs1;
    Register Rd;
    uint32_t Funct3;

    /// @brief Construction takes an encoded instruction
    /// and unpacks the components.
    Itype(uint32_t inst) {
        Imm    = ((int64_t)(int32_t)(inst & 0xfff00000)) >> 20;
        Rs1    = Register((inst >> 15) & 0b11111);
        Rd     = Register((inst >> 7) & 0b11111);
        Funct3 = (inst >> 12) & 0b111;
    }
};

/// @brief Stype instructions.
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

/// @brief Jtype instructions.
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

/// @brief Btype instructions.
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

/// @brief Utype instructions.
struct Utype {
    int32_t Imm;
    Register Rd;

    Utype(uint32_t inst) {
        Imm = (int32_t)(inst & 0xfffff000);
        Rd  = Register((inst >> 7) & 0b11111);
    }
};

/// @brief Instruction represents RISC-V instructions as described
/// in the ISA.
/// Instructions are 32 bits wide and encoded in Little Endian format
/// the ISA specifies the first 7 bits to be the OPCode but in RISC-V
/// OPCode don't directly map to instructions but rather an instruction
/// group, for example 0b0010011 groups (addi, slti, sltiu, xori, ori,andi...)
/// Instruction simply wraps the bits of an encoded instruction, during
/// the decode cycle the instruction is transformed to its appropiate type.
struct Instruction {
    uint32_t opcode;
    uint32_t instruction;

    /// Following C++ Guidelines and from intuition we know
    /// instruction will be an invariant since the struct merely
    /// wraps the value.
    /// So besides default constructors the class will have a single
    /// explicit constructor that takes the encoded version of the instruction
    /// and explicitly decode it.

    /// When moving to RV64I increase to 64 bits
    /// this constructor is the one used to decode
    /// a given 4 bytes encoded instruction.

    auto GetOpcode() -> uint8_t;
    [[nodiscard]] auto ToString() const -> std::string;
};
} // namespace riscvemu

#endif

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <cassert>
#include <cstdint>
#include <string>

namespace riscvemu {

/// @brief Register enum encodes the ABI name for RISC-V registers.
/// The ABI names are described in the RISC-V calling conventions.
enum class Register {
    Zero = 0, // Hard-wired zero
    Ra,       // Return address
    Sp,       // x2 is used as the stack pointer
    Gp,       // Global pointer
    Tp,       // Thread pointer
    T0,       // Temporary
    T1,       // Temporary T1
    T2,       // Temporary T2
    S0,       // Saved register (frame pointer)
    S1,       // Saved register
    A0,       // Function arguments / Return values
    A1,       // Function arguments
    A2,       // Function arguments
    A3,       // Function arguments
    A4,       // Function arguments
    A5,       // Function arguments
    A6,       // Function arguments
    A7,       // Function arguments
    S2,       // Saved registers
    S3,       // Saved registers
    S4,       // Saved registers
    S5,       // Saved registers
    S6,       // Saved registers
    S7,       // Saved registers
    S8,       // Saved registers
    S9,       // Saved registers
    S10,      // Saved registers
    S11,      // Saved registers
    T3,       // Temporary T3
    T4,       // Temporary T4
    T5,       // Temporary T5
    T6,       // Temporary T6
    Pc,       // Program counter  (not explicitely declared)
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

/// @brief OPCode enum represents each OPCode enum with its binary
/// representation. For readability we list all instructions within
///  For readability we list all instructions within
///  an opcode group.
enum class OPCode {

    /// The following are the opcodes for the base RV32 ISA.

    // Load Upper Immediate.
    LUI = 0b0110111,
    // Add Upper immediate to PC.
    AUIPC = 0b0010111,

    // Unconditional jumps opcodes.

    // Jump And Link.
    JAL = 0b1101111,
    // Jump And Link Register.
    JALR = 0b1100111,

    // Conditional branch opcodes.
    BRANCH = 0b1100011,
    // Branch if Equal.
    BEQ = 0b1100011,
    // Branch if Not Equal.
    BNE = 0b1100011,
    // Branch if Less Than.
    BLT = 0b1100011,
    // Branch if Greater than or Equal.
    BGE = 0b1100011,
    // Branch if Less Than Upper.
    BLTU = 0b1100011,
    // Branch if Greater than or Equal Upper.
    BGEU = 0b1100011,

    // Load opcodes.
    LOAD = 0b0000011,
    // Load byte.
    LB = 0b0000011,
    // Load half word.
    LH = 0b0000011,
    // Load word.
    LW = 0b0000011,
    // Load byte upper.
    LBU = 0b0000011,
    // Load half word upper.
    LHU = 0b0000011,

    // Store opcodes.
    STORE = 0b0100011,
    // Store byte,
    SB = 0b0100011,
    // Store half word.
    SH = 0b0100011,
    // Store word.
    SW = 0b0100011,

    // Arithmetic immediate operations.
    ARITHI = 0b0010011,
    // Add Immediate.
    ADDI = 0b0010011,
    // Set Less Than Immediate.
    SLTI = 0b0010011,
    // Set Less Than Immediate Unsigned.
    SLTIU = 0b0010011,
    // Xor Immediate.
    XORI = 0b0010011,
    // Or Immediate.
    ORI = 0b0010011,
    // And Immedaite
    ANDI = 0b0010011,
    // Shift Logical Left Immediate.
    SLLI = 0b0010011,
    // Shift Logical Right Immediate.
    SRLI = 0b0010011,
    // Shift Right Arithmetic
    SRAI = 0b0010011,

    // Arithmetic immediate operations for wide registers (RV64I).
    ARITHIW = 0b0011011,
    // ADDIW: Add Immediate Wide.
    ADDIW = 0b0011011,
    SLLIW = 0b0011011,
    SRLIW = 0b0011011,
    SRAIW = 0b0011011,

    // Arithmetic register to register operations for wide registers (RV64I).
    ARITHRW = 0b0111011,
    // Add Wide
    ADDW = 0b0111011,
    // Substract Wide
    SUBW = 0b0111011,
    // Shift Left Logical Wide.
    SLLW = 0b0111011,
    // Shift Right Logical Wide.
    SRLW = 0b0111011,
    // Shift Right Arithmetic Wide.
    SRAW = 0b0111011,

    // Arithmetic register to register operations.
    ARITHR = 0b0110011,
    // Add
    ADD = 0b0110011,
    // Sub
    SUB = 0b0110011,
    // Shift Logical Left.
    SLL = 0b0110011,
    // Set Less Than
    SLT = 0b0110011,
    // Set Less Than Upper
    SLTU = 0b0110011,
    // XOR
    XOR = 0b0110011,
    // Shift Logical Right.
    SRL = 0b0110011,
    // Shift Arithmetic Right.
    SRA = 0b0110011,
    // OR
    OR = 0b0110011,
    // AND
    AND = 0b0110011,

    // Memory ordering

    // Fence instruction.
    FENCE = 0b0001111,

    // Environment calls and breakpoints, system instructions used
    // to access system functionality that might require priviliegd
    // access.
    // ECALL
    ECALL  = 0b1110011,
    EBREAK = 0b1110011,

    /// @brief the following are the extended base instructions for RV64I

    // Load operations opcodes.

    // Load Word Upper
    LWU = 0b0000011,
    // Load Double Word.
    LD = 0b0000011,

    // Store operations opcodes.
    // Store double.
    SD = 0b0100011,

    // Control and Status registers opcodes.
    CSR = 0b1110011,

    CSRRW  = 0b1110011,
    CSRRS  = 0b1110011,
    CSRRC  = 0b1110011,
    CSRRWI = 0b1110011,
    CSRRSI = 0b1110011,
    CSRRI  = 0b1110011,
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
    OPCode opcode;
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

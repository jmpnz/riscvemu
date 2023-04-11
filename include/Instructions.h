#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <_types/_uint32_t.h>
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
  T0,
  T1,
  T2,
  S0,
  S1,
  A0,
  A1,
  A2,
  A3,
  A4,
  A5,
  A6,
  A7,
  S2,
  S3,
  S4,
  S5,
  S6,
  S7,
  S8,
  S9,
  S10,
  S11,
  T3,
  T4,
  T5,
  T6,
  Pc,
};

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
    Rs2 = Register((inst >> 20) & 0b11111);
    Rs1 = Register((inst >> 15) & 0b11111);
    Funct3 = (inst >> 12) & 0b111;
    Rd = Register((inst >> 7) & 0b11111);
  }
};

/// @brief Stype instructions.
struct Stype {};

/// @brief Instruction represents RISC-V instructions as described
/// in the ISA.
/// Instructions are 32 bits wide and encoded in Little Endian format.
/// [6..0] => OPCode
/// [19..15] => RS1
struct Instruction {
  uint8_t opcode_;
  uint64_t instruction;

  /// Following C++ Guidelines and from intuition we know
  /// instruction will be an invariant since the struct merely
  /// wraps the value.
  /// So besides default constructors the class will have a single
  /// explicit constructor that takes the encoded version of the instruction
  /// and explicitly decode it.

  /// When moving to RV64I increase to 64 bits
  /// this constructor is the one used to decode
  /// a given 4 bytes encoded instruction.
  Instruction(uint32_t bits);

  auto GetOpcode() -> uint8_t;
  [[nodiscard]] auto ToString() const -> std::string;
};
} // namespace riscvemu

#endif

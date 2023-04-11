#ifndef MACHINE_H
#define MACHINE_H

#include "Instructions.h"

#include <cstdint>

#include <array>
#include <memory>
#include <utility>
#include <vector>

namespace riscvemu {

using offset_t = size_t;

/// @brief MMU represents a memory management unit, used to handle address
/// translations for virtual memory.
struct MMU {
  // Raw memory buffer.
  std::vector<uint8_t> memory;
  // Used memory.
  size_t used;
};

/// @brief Instructions is a typedef for the current file.
using CPUInstructions = std::vector<uint8_t>;

/// @brief VMContext is the virtual machine execution context, you can think
/// of it as thread spaces, used for context switching and interrupt
/// handling.
// VMContext stores the code executing in a context.
struct VMContext {
  CPUInstructions code;

  /// @brief VMContext constructor.
  VMContext(std::vector<uint8_t> code) : code(std::move(code)) {}
};

/// @brief CPU represents the CPU unit in the emulator, it's responsible
/// for the entire pipeline cycle (fetch, decode, execute).
/// CPU owns MMU exclusively, any simulated threading is done at the context
/// level.
class CPU {

public:
  /// @brief CPU instance constructor.
  CPU(VMContext ctx) : ctx(std::make_unique<VMContext>(std::move(ctx))) {}

  /// @brief Fetch and decode an instruction, and increment program counter.
  auto FetchDecode() -> Instruction;

  /// @brief Fetch an encoded instruction.
  auto Fetch() -> uint32_t;

  /// @brief Execute a single instruction.
  auto Execute(uint32_t instruction) -> void;

  /// @brief Dump register states.
  auto DumpRegisters() -> void;

  /// @brief Run the CPU instance, asserts if memory is non-empty and has
  /// instructions.
  auto Run() -> void;

  /// @brief Returns the current program counter.
  auto GetPC() const -> uint64_t;

private:
  /// @brief  Program counter,
  /// TODO: maybe start at an actual offset in MMU ?
  offset_t pc = 0;
  /// @brief Registers.
  std::array<uint32_t, 32> registers;

  /// @brief CPU instance contexts.
  std::unique_ptr<VMContext> ctx;
};

} // namespace riscvemu

#endif

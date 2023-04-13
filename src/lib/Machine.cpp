#include "Machine.h"

#include <_types/_uint32_t.h>

#include <cstdint>
#include <iostream>

#include "Instructions.h"

namespace riscvemu {

//=== MMU Methods Implementations ====//

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

auto MMU::load8(uint64_t addr) -> uint64_t {
  return (uint64_t)this->memory[addr - MemoryBaseAddr];
}

auto MMU::load16(uint64_t addr) -> uint64_t {
  return (uint64_t)this->memory[addr - MemoryBaseAddr] |
         (uint64_t)this->memory[addr - MemoryBaseAddr + 1] << 8;
}

auto MMU::load32(uint64_t addr) -> uint64_t {
  return (uint64_t)this->memory[addr - MemoryBaseAddr] |
         (uint64_t)this->memory[addr - MemoryBaseAddr + 1] << 8 |
         (uint64_t)this->memory[addr - MemoryBaseAddr + 2] << 16 |
         (uint64_t)this->memory[addr - MemoryBaseAddr + 3] << 24;
}

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

//==== CPU Methods Implementations ====//

/// @brief Fetches the next instruction to execute stored @ pc.
/// @return Returns the instruction in 32-bit format (encoded.)
auto CPU::fetch() -> uint32_t {
  // Fetch index of next instruction to execute in code.
  auto index = this->pc;
  // Instructions are encoded in Little Endian format
  // We encode the instruction to its 32 bit format here.
  uint32_t const r1 = this->ctx->code.at(index);
  uint32_t const r2 = (this->ctx->code.at(index + 1) << 8);
  uint32_t const r3 = (this->ctx->code.at(index + 2) << 16);
  uint32_t const r4 = (this->ctx->code.at(index + 3) << 24);

  return (r1 | r2 | r3 | r4);
}

auto CPU::GetPC() const -> uint64_t { return this->pc; }

auto CPU::DumpRegisters() -> void {
  for (int i = 0; i < 32; i++) {
    std::cout << "x" << i << " = [ " << this->registers[i] << " ] " << '\n';
  }
  std::cout << std::endl;
}

void CPU::Execute(uint32_t inst) {
  // Fetch next instruction.
  // Can be wrapped in fetchDecode function and do both.
  // auto instruction = this->decodeAtAddress(this->pc);
  // return instruction;
  // Execute instruction.
  // Ideally either have execute be a giant
  // swtich case or do some nifty design
  // make it super simple at first.
  // this->ctx->execute(instruction).
  std::cout << "Executing Instruction: " << inst << " @ " << this->pc << '\n';
  auto opcode = inst & 0x7f;
  uint64_t const register_rd = ((inst >> 7) & 0x1f);
  uint64_t const register_rs1 = ((inst >> 15) & 0x1f);
  uint64_t const register_rs2 = ((inst >> 20) & 0x1f);

  std::cout << "Opcode :" << opcode << '\n';

  switch (opcode) {
  case 0x13: {
    // addi
    uint64_t const imm = (uint64_t(uint32_t((inst & 0xfff00000))) >> 20);
    std::cout << "Register RD :" << register_rd
              << " Contents :" << this->registers[register_rd] << '\n';
    this->registers[register_rd] = (this->registers[register_rs1] + imm);
    break;
  }
  case 0x33: {
    // add
    this->registers[register_rd] =
        (this->registers[register_rs1] + this->registers[register_rs2]);

    std::cout << "Register RD :" << register_rd
              << " Contents :" << this->registers[register_rd] << '\n';

    break;
  }
  default: {
    std::cerr << "Unknown instruction " << opcode << '\n';
    break;
  }
  }

  std::cout << "OPCode: " << opcode << " [RD] = " << register_rd
            << " [RS1] = " << register_rs1 << '\n';
};

void CPU::Run() {
  while (this->pc < this->ctx->code.size()) {
    auto inst = this->fetch();
    this->pc += 4;
    this->Execute(inst);
  }
}

} // namespace riscvemu

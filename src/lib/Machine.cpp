#include "Machine.h"
#include "Instructions.h"

#include <_types/_uint32_t.h>
#include <cstdint>
#include <iostream>

namespace riscvemu {

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

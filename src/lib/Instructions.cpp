#include "Instructions.h"
#include <cstdint>
#include <string>

namespace riscvemu {

auto Instruction::ToString() const -> std::string {
    switch (this->opcode_) {
    case 0xFF:
        return "OPCODE: 0xFF";
    default:
        return "Unknown instruction";
    }
}

} // namespace riscvemu

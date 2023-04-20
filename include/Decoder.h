#ifndef DECODER_H
#define DECODER_H

#include "Instructions.h"
#include <cstdint>

namespace riscvemu {

// Mask for decoding opcode from instruction bits, opcodes are stored
// in the first 7 bits.
static constexpr uint32_t OPCodeMask = 0x7f;

//Decode an instruction given its type.
template <typename T> auto decodeInstruction(const uint32_t instruction) -> T {
    return T(instruction);
}
}; // namespace riscvemu

#endif

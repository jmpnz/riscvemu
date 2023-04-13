#ifndef MACHINE_H
#define MACHINE_H

#include "Instructions.h"

#include <cstdint>

#include <array>
#include <memory>
#include <utility>
#include <vector>

namespace riscvemu {

static constexpr int MemoryMaxSize  = 1024 * 1024;
static constexpr int MemoryBaseAddr = 0x80000000;

using offset_t = size_t;

/// @brief MMU represents a memory management unit, used to handle address
/// translations for virtual memory.
struct MMU {
    // Raw memory buffer.
    uint8_t memory[MemoryMaxSize];
    // Used memory.
    size_t used;

    /// @brief Base address for the MMU, fixed constant see MemoryBaseAddr.
    /// @return constant integer value MemoryBaseAddr.
    static constexpr int baseAddress() { return MemoryBaseAddr; }

    /// @brief MMU max available unmapped memory, fixed constant see
    /// MemoryMaxSize.
    /// @return constant integer value MemoryMaxSize.
    static constexpr int memorySize() { return MemoryMaxSize; }

    /// @brief Load size number of bits at address addr, size must be within
    /// addressable range i.e (8, 16, 32, 64).
    /// @param addr
    /// @param size
    /// @return size number of bits stored at address addr.
    uint64_t load(uint64_t addr, uint64_t size);

    /// @brief Store size number of bits in value at address addr, size must be
    /// within addressable range i.e (8, 16, 32, 64).
    /// @param addr
    /// @param size
    /// @param value
    void store(uint64_t addr, uint64_t size, uint64_t value);

    private:
    /// @brief Load 8 bits at address addr.
    /// @param addr
    /// @return 8 bit long value stored at addr as uin64_t.
    uint64_t load8(uint64_t addr);

    /// @brief Load 16 bits at address addr.
    /// @param addr
    /// @return 16 bit long value stored at addr as uin64_t
    uint64_t load16(uint64_t addr);

    /// @brief Load 32 bits at address addr.
    /// @param addr
    /// @return 32 bit long value stored at addr as uin64_t
    uint64_t load32(uint64_t addr);

    /// @brief Load 64 bits at address addr.
    /// @param addr
    /// @return 64 bit long value stored at addr as uin64_t
    uint64_t load64(uint64_t addr);

    /// @brief Store 8 bit value at address addr.
    /// @param addr
    /// @param value
    void store8(uint64_t addr, uint64_t value);

    /// @brief Store 16 bit value at address addr.
    /// @param addr
    /// @param value
    void store16(uint64_t addr, uint64_t value);

    /// @brief Store 32 bit value at address addr.
    /// @param addr
    /// @param value
    void store32(uint64_t addr, uint64_t value);

    /// @brief Store 64 bit value at address addr.
    /// @param addr
    /// @param value
    void store64(uint64_t addr, uint64_t value);
};

/// @brief VMContext is the virtual machine execution context, you can think
/// of it as thread spaces, used for context switching and interrupt
/// handling.
// VMContext stores the code executing in a context.
struct VMContext {
    std::vector<uint8_t> code;

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
    /// @brief Fetch instruction at current program counter.
    // this implies each n+1 read is shifted by 8 bytes.
    auto fetch() -> uint32_t;

    /// @brief Decode an encoded instruction.
    auto decode(uint32_t instruction) -> Instruction;

    /// @brief Execurte an instruction.
    auto execute(const Instruction& inst) -> void;

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

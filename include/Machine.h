#ifndef MACHINE_H
#define MACHINE_H

#include "Instructions.h"

#include <cstddef>
#include <cstdint>

#include <array>
#include <memory>
#include <utility>
#include <vector>

namespace riscvemu {

/// @brief Offset is used to represent memory offsets.
using offset_t = size_t;

/// @brief VirtualAddress is defined to be an uint64_t asour emulator aims
/// to implement the RV64I ISA.
using VirtualAddress = uint64_t;

/// @brief MMU will have 128 MiB.
static constexpr uint64_t MemoryMaxSize =
    (static_cast<const uint64_t>(1024 * 1024 * 1));

/// @brief MMU Base address.
static constexpr VirtualAddress MemoryBaseAddr = 0x80000000;

/// @brief MMU End address.
static constexpr uint64_t MemoryEndAddr = MemoryMaxSize + MemoryBaseAddr - 1;

/// @brief LoadAccessFault exception used to handle memory access beyond
/// virtual memory bounds.
struct LoadAccessFault : public std::exception {
    int code = 5;

    [[nodiscard]] auto what() const noexcept -> const char* override {
        return "Load access fault";
    }
};

/// @brief IllegalInstruction exception used to handle unimplemented
/// or illegal instructions.
struct IllegalInstruction : public std::exception {
    int code = 2;

    [[nodiscard]] auto what() const noexcept -> const char* override {
        return "Illegal Instruction";
    }
};

/// @brief MMU represents a memory management unit, used to handle address
/// translations for virtual memory.
struct MMU {
    // Raw memory buffer.
    std::vector<uint8_t> memory; // NOLINT
    // Used memory.
    size_t used;

    /// @brief Default constructor allocates MemoryMaxSize in the underlying
    // buffer.
    MMU() : memory(MemoryMaxSize) {}

    /// @brief Checks if a given address is within the virtual memory
    /// accessible range.
    /// @param addr
    /// @return boolean
    static constexpr auto withinRange(VirtualAddress addr) -> bool {
        return (addr >= MemoryBaseAddr && addr <= MemoryEndAddr);
    }

    /// @brief Base address for the MMU, fixed constant see MemoryBaseAddr.
    /// @return constant integer value MemoryBaseAddr.
    static constexpr auto baseAddress() -> uint64_t { return MemoryBaseAddr; }

    /// @brief MMU max available unmapped memory, fixed constant see
    /// MemoryMaxSize.
    /// @return constant integer value MemoryMaxSize.
    static constexpr auto memorySize() -> int { return MemoryMaxSize; }

    /// @brief Dump MMU contents starting from base address.
    auto dumpMemory() -> void;

    /// @brief Load size number of bits at address addr, size must be within
    /// addressable range i.e (8, 16, 32, 64).
    /// @param addr
    /// @param size
    /// @return size number of bits stored at address addr.
    auto load(VirtualAddress addr, size_t size) -> uint64_t;

    /// @brief Store size number of bits in value at address addr, size must be
    /// within addressable range i.e (8, 16, 32, 64).
    /// @param addr
    /// @param size
    /// @param value
    void store(VirtualAddress addr, size_t size, uint64_t value);

    /// @brief Load 8 bits at address addr.
    /// @param addr
    /// @return 8 bit long value stored at addr as uin64_t.
    auto load8(VirtualAddress addr) -> uint64_t;

    /// @brief Load 16 bits at address addr.
    /// @param addr
    /// @return 16 bit long value stored at addr as uin64_t
    auto load16(VirtualAddress addr) -> uint64_t;

    /// @brief Load 32 bits at address addr.
    /// @param addr
    /// @return 32 bit long value stored at addr as uin64_t
    auto load32(VirtualAddress addr) -> uint64_t;

    /// @brief Load 64 bits at address addr.
    /// @param addr
    /// @return 64 bit long value stored at addr as uin64_t
    auto load64(VirtualAddress addr) -> uint64_t;

    /// @brief Store 8 bit value at address addr.
    /// @param addr
    /// @param value
    void store8(VirtualAddress addr, uint64_t value);

    /// @brief Store 16 bit value at address addr.
    /// @param addr
    /// @param value
    void store16(VirtualAddress addr, uint64_t value);

    /// @brief Store 32 bit value at address addr.
    /// @param addr
    /// @param value
    void store32(VirtualAddress addr, uint64_t value);

    /// @brief Store 64 bit value at address addr.
    /// @param addr
    /// @param value
    void store64(VirtualAddress addr, uint64_t value);
};

/// @brief VMContext is the virtual machine execution context, you can think
/// of it as thread spaces, used for context switching and interrupt
/// handling.
// VMContext stores the code executing in a context.
// TODO: get rid of VMContext until we figure out multithreaded
// execution.
struct VMContext {
    std::vector<uint8_t> code;

    /// @brief MMU for CPU execution.
    MMU mmu;

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
    CPU(VMContext ctx) : ctx(std::make_unique<VMContext>(std::move(ctx))) {
        // Register x0 is always hardwired to 0.
        this->registers[0] = 0x00;
        /// Register x2 is used as the stack pointer by the ABI.
        this->registers[2] = MemoryBaseAddr + MemoryMaxSize - 4;
        /// Program counter is set to the base memory address.
        this->pc = MemoryBaseAddr;
        /// Move code to MMU.
        std::memcpy(this->ctx->mmu.memory.data(), this->ctx->code.data(),
                    this->ctx->code.size());
    }

    /// @brief Return program counter.
    [[nodiscard]] auto inline getPC() const -> uint64_t { return pc; }

    /// @brief Run the CPU instance, asserts if memory is non-empty and has
    /// instructions.
    auto run() -> void;

    /// @brief Fetch instruction at current program counter.
    // this implies each n+1 read is shifted by 8 bytes.
    auto fetch() -> uint32_t;

    /// @brief Decode an encoded instruction.
    auto static decode(uint32_t instruction) -> Instruction;

    /// @brief Execute an instruction.
    auto execute(const Instruction& inst) -> void;

    /// @brief Get value stored in register.
    auto getRegister(Register reg) -> uint64_t;

    /// @brief Set register with value.
    auto setRegister(Register reg, uint64_t value) -> void;

    /// @brief Dump register states.
    auto dumpRegisters() -> void;

    /// @brief Load size number of bytes at address.
    /// @param addr
    /// @param size
    /// @return value at address upcast to uint64_t.
    auto load(VirtualAddress addr, size_t size) -> uint64_t;

    /// @brief Store size number of bytes from value at address.
    /// @param addr
    /// @param size
    /// @param value
    /// @return void
    auto store(VirtualAddress addr, size_t size, uint64_t value) -> void;

    private:
    /// @brief  Program counter,
    /// TODO: maybe start at an actual offset in MMU ?
    offset_t pc = MemoryBaseAddr;
    /// @brief Registers.
    std::array<uint64_t, 32> registers;

    /// @brief CPU instance contexts.
    std::unique_ptr<VMContext> ctx;
};

} // namespace riscvemu

#endif

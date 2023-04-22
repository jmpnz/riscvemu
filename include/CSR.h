#ifndef CSR_H
#define CSR_H

#include <cstdint>

namespace riscvemu {
// Control status registers.
//
// Machine information registers.

// Hardware thread id.
static constexpr uint64_t MHartID = 0xf14;

// Machine trap setup registers.

// Machine status register.
static constexpr uint64_t MStatus = 0x300;
// ISA and extensions
static constexpr uint64_t MIsa = 0x301;

// Machine exception delegation register.
static constexpr uint64_t MEDeleg = 0x302;

// Machine interrupt delegation register.
static constexpr uint64_t MIDeleg = 0x303;

// Machine interrupt-enable register.
static constexpr uint64_t MIE = 0x304;

// Machine trap-handler base address
static constexpr uint64_t MTVec = 0x305;

// Machine counter enable.
static constexpr uint64_t MCounteren = 0x306;

// Machine handling registers.

// Scratch register for machine trap handlers.
static constexpr uint64_t MScratch = 0x340;

// Machine exception program counter.
static constexpr uint64_t MEPc = 0x341;

// Machine trap cause cause..
static constexpr uint64_t MCause = 0x342;

// Machine bad address or instruction.
static constexpr uint64_t MTVal = 0x343;

// Machine interrupt pending.
static constexpr uint64_t MIp = 0x344;

// Machine trap instruction
static constexpr uint64_t MTInst = 0x34A;

// Machine bad guest physical address.
static constexpr uint64_t MTVal2 = 0x34B;

// Supervisor registers.

// Supervisor trap setup registers.
static constexpr uint64_t SStatus = 0x100;

// Supervisor interrupt-enable register.
static constexpr uint64_t Sie = 0x104;

// Supervisor trap handler base address.
static constexpr uint64_t STVec = 0x105;

// Supervisor  counter enable..
static constexpr uint64_t SCounteren = 0x106;

// Supervisor trap handling.

// Scratch register for supervisor trap handlers.
static constexpr uint64_t SSCratch = 0x140;

// Supervisor exception program counter.
static constexpr uint64_t Sepc = 0x141;

// Scratch trap cause.
static constexpr uint64_t SCause = 0x142;

// Supervisor bad address or instruction.
static constexpr uint64_t STVal = 0x143;

// Supervisor interrupt pending.
static constexpr uint64_t Sip = 0x144;

// Supervisor address translation and protection.
static constexpr uint64_t Satp = 0x180;

// Supervisor-mode context register.
static constexpr uint64_t SContext = 0x5A8;

static constexpr uint64_t MaskSIE  = 1 << 1;
static constexpr uint64_t MaskMIE  = 1 << 3;
static constexpr uint64_t MaskSPIE = 1 << 5;
static constexpr uint64_t MaskUBE  = 1 << 6;
static constexpr uint64_t MaskMPIE = 1 << 7;
static constexpr uint64_t MaskSPP  = 1 << 8;
static constexpr uint64_t MaskVS   = 3 << 9;
static constexpr uint64_t MaskMPP  = 3 << 11;
static constexpr uint64_t MaskFS   = 3 << 13;
static constexpr uint64_t MaskXS   = 3 << 15;
static constexpr uint64_t MaskMPRV = 1 << 17;
static constexpr uint64_t MaskSUM  = 1 << 18;
static constexpr uint64_t MaskMXR  = 1 << 19;
static constexpr uint64_t MaskTVM  = 1 << 20;
static constexpr uint64_t MaskTW   = 1 << 21;
static constexpr uint64_t MaskTSR  = 1 << 22;
static constexpr uint64_t MaskUXL  = 3ULL << 32;
static constexpr uint64_t MaskSXL  = 4ULL << 34;
static constexpr uint64_t MaskSBE  = 1ULL << 36;
static constexpr uint64_t MaskMBE  = 1ULL << 37;
static constexpr uint64_t MaskSD   = 1ULL << 63;

// MIP / SIP field mask
static constexpr uint64_t MaskSSIP    = 1 << 1;
static constexpr uint64_t MaskMSIP    = 1 << 3;
static constexpr uint64_t MaskSTIP    = 1 << 5;
static constexpr uint64_t MaskMTIP    = 1 << 7;
static constexpr uint64_t MaskSEIP    = 1 << 9;
static constexpr uint64_t MaskMEIP    = 1 << 11;
static constexpr uint64_t MaskSSTATUS = MaskSIE | MaskSPIE | MaskUBE | MaskSPP |
                                        MaskFS | MaskXS | MaskSUM | MaskMXR |
                                        MaskUXL | MaskSD;

struct CSR {
    uint64_t csrs[4096]; //NOLINT

    // Load and Store operations on each Control and Status registers.

    /// Return value stored at the register specified in addr.
    auto load(uint64_t addr) -> uint64_t {
        if (addr == Sie) {
            return this->csrs[MIE] & this->csrs[MIDeleg];
        }
        if (addr == Sip) {
            return this->csrs[MIp] & this->csrs[MIDeleg];
        }
        if (addr == SStatus) {
            return this->csrs[MStatus] & MaskSSTATUS;
        }
        return this->csrs[addr];
    }

    /// Store value at the register specified in addr.
    auto store(uint64_t addr, uint64_t value) -> void {
        if (addr == Sie) {
            this->csrs[MIE] =
                (this->csrs[MIE] & ~this->csrs[MIDeleg]) | //NOLINT
                (value & this->csrs[MIDeleg]);
        }
        if (addr == Sip) {
            this->csrs[MIp] =
                (this->csrs[MIE] & ~this->csrs[MIDeleg]) | //NOLINT
                (value & this->csrs[MIDeleg]);
        }
        if (addr == SStatus) {
            this->csrs[MStatus] =
                (this->csrs[MStatus] & ~MaskSSTATUS) | //NOLINT
                (value & MaskSSTATUS);
        }
        this->csrs[addr] = value;
    }
};

}; // namespace riscvemu
#endif

#include "Instructions.h"

#include <cstdint>
#include <string>

namespace riscvemu {

/// @brief Returns ABI name of register as a string
/// @param r
/// @return const char*
auto getRegisterABIName(Register r) -> const char* {
    switch (r) {
    case Register::Zero:
        return "zero";
    case Register::Ra:
        return "ra";
    case Register::Sp:
        return "sp";
    case Register::Gp:
        return "gp";
    case Register::Tp:
        return "tp";
    case Register::T0:
        return "t0";
    case Register::T1:
        return "t1";
    case Register::T2:
        return "t2";
    case Register::S0:
        return "s0";
    case Register::S1:
        return "s1";
    case Register::A0:
        return "a0";
    case Register::A1:
        return "a1";
    case Register::A2:
        return "a2";
    case Register::A3:
        return "a3";
    case Register::A4:
        return "a4";
    case Register::A5:
        return "a5";
    case Register::A6:
        return "a6";
    case Register::A7:
        return "a7";
    case Register::S2:
        return "s2";
    case Register::S3:
        return "s3";
    case Register::S4:
        return "s4";
    case Register::S5:
        return "s5";
    case Register::S6:
        return "s6";
    case Register::S7:
        return "s7";
    case Register::S8:
        return "s8";
    case Register::S9:
        return "s9";
    case Register::S10:
        return "s10";
    case Register::S11:
        return "s11";
    case Register::T3:
        return "t3";
    case Register::T4:
        return "t4";
    case Register::T5:
        return "t5";
    case Register::T6:
        return "t6";
    default:
        return "Unknown";
    }
}

/// @brief Return typed Register value from register index.
/// @param idx
/// @return Register (ABI) corresponding to index.
auto getRegisterFromIndex(uint64_t idx) -> Register {
    switch (idx) {
    case 0:
        return Register::Zero;
    case 1:
        return Register::Ra;
    case 2:
        return Register::Sp;
    case 3:
        return Register::Gp;
    case 4:
        return Register::Tp;
    case 5:
        return Register::T0;
    case 6:
        return Register::T1;
    case 7:
        return Register::T2;
    case 8:
        return Register::S0;
    case 9:
        return Register::S1;
    case 10:
        return Register::A0;
    case 11:
        return Register::A1;
    case 12:
        return Register::A2;
    case 13:
        return Register::A3;
    case 14:
        return Register::A4;
    case 15:
        return Register::A5;
    case 16:
        return Register::A6;
    case 17:
        return Register::A7;
    case 18:
        return Register::S2;
    case 19:
        return Register::S3;
    case 20:
        return Register::S4;
    case 21:
        return Register::S5;
    case 22:
        return Register::S6;
    case 23:
        return Register::S7;
    case 24:
        return Register::S8;
    case 25:
        return Register::S9;
    case 26:
        return Register::S10;
    case 27:
        return Register::S11;
    case 28:
        return Register::T3;
    case 29:
        return Register::T4;
    case 30:
        return Register::T5;
    case 31:
        return Register::T6;
    default:
        return Register::Unknown;
    }
}

} // namespace riscvemu

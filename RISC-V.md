# RISC-V Intrusction Set

Notes about RISC-V.

##  RISC-V ISA

RISC-V ISA consists of modules, a base integer ISA plus optional extensions.
The base integer ISA must be implemented in any platform. There are 2 variants
in the base integer ISA, RV32I for a 32-bit architecture and RV64I for a 64-bit
architecture.

The base integer ISA only contains simple integer arithmetic operations
so it's not enough so that it can't run complex systems such as OSes.

RISC-V defines a combination of a base ISA plus selected extensions
as a "general-purpose" ISA, called RV32G or RV64G, to allow complex
systems to run on RISC-V hardware.

Here is the list of ISAs that RV64G includes:

* RV64I: base integer instructions
* RV64M: integer multiplication and division instructions
* RV64A: atomic instructions
* RV64F: single-precision floating-point instructions
* RV64D: double-precision floating-point instructions
* RVZicsr: control and status register instructions
* RVZifencei: instruction-fetch fence instructions

For our emulator we are starting with the RV64I ISA, the largest difference
between RV32I and RV64I is that registers in RV64I are 64 bits wide instead
of 32 bits wide. The are other differences with regards to the opcodes encoding
formats as well (discussed later on). Besides encoding RV64I arithmetic is done
with 64 bit wide values.

### RISC-V Instruction Encoding

RISC-V RV64I has 6 instruction encoding formats (R/I/S/U/B/J), RISC-V encoding
formats share many attributes :

* The first 7 bits are reserved for the opcode, with one opcode representing
a group of instructions.

* All instructions are 32 bits wide even for RV64I.

* Each instruction format involves at least one register operand, the destination
register is encoded in Rd with Rs1 and Rs2 being the source registers.

* All register operands are 5 bit long.

For each instruction format the binary layout (how instructions are encoded) is as follows :

The first line describes the bit index, since all instructions are 32 bit wide offsets are
in the range [0..31].

Note: To keep the ASCII diagram nice and cleann we omit Imm where it's not required e.g
for B type instruction [4:1|11]] is synonymous to Imm[4:1] Imm[11] the bit right after
the opcode is the 11th bit of the immediate value, with [4:1] representing bits 1 to 4.

```

31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0

[      Funct7      ] [     Rs2     ] [    Rs1   ] [Funct 3] [   Rd   ] [   Opcode   ]  R

[             Imm[11:0]            ] [    Rs1   ] [Funct 3] [   Rd   ] [   Opcode   ]  I

[    Imm[11:5]     ] [     Rs2     ] [    Rs1   ] [Funct 3] [Imm[4:0]] [   Opcode   ]  S

[Imm[12] Imm[10:5] ] [     Rs2     ] [    Rs1   ] [Funct 3] [[4:1|11]] [   Opcode   ]  B

[                       Imm[31:12]                        ] [   Rd   ] [   Opcode   ]  U

[Imm[20]             Imm[10:1]     ] [ Imm[11]  ] [[19:12]] [   Rd   ] [   Opcode   ]  J

```

To give a practical example, let's consider `addw` instruction which adds two wide
operands and stores the result in rd.

For simplicity let's assume our instruction `addw` is stored at address 0x1000
we read 4 bytes starting from 0x1000 to 0x1004 and let's lay them out in binary.

```

[00111011] [00000101] [10110101] [00000000]

```

We assume a little-endian layout so the LSB is stored at 0x1000 and MSB at 0x1004
before we start decoding we need to flip the above bits

```

[00000000] [10110101] [00000101] [00111011]

```

`addw` is an Rtype instruction so let's actually decode the above instruction :

```

 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0

[ 0  0  0  0  0  0  0|0  1  0  1  1 | 0  1  0  1  0| 0  0  0| 0  1 0 1 0|0 1 1 1 0 1 1

        Funct7              Rs2            Rs1        Funct3      Rd        Opcode

```

`addw` by definition stores the sum of `rs1` and rs2` in `rd` you'll notice that `rd`
has the same bit value as `rs1` and this is because we store the result back into `rs1`.

In assembly and using the RISC-V ABI this would be written as `addw a0, a0 ,a1` the ABi
defines that the return value is stored in register `a0`.

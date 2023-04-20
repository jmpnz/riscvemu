# riscvemu

`riscvemu` is a RISC-V emulator written in C++.

The goal of the emulator is to act as a VM for another project I am working on.

## Build

You'll need CMake & Ninja

```sh

$ mkdir build/
$ cd build
$ cmake ..
$ ninja all

```

## Style

We use LLVM's style guide for C++ code (unlike LLVM code we allow exceptions).


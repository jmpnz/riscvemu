# riscvemu

`riscvemu` is a RISC-V emulator written in C++.

The goal of the emulator is to act as a VM for another project
I am working on.

## Build

You'll need CMake & Ninja for building the main emulator binary
which has limited functionality, to build tests you'll need a RISC-V
toolchain.

I personally used `riscv-gnu-toolchain` which builds `gcc` and other
binutils that might be useful for debugging such as `objcopy` & `objdump`.

To build the main binary you can run the following.

```sh

$ mkdir build/
$ cd build
$ cmake ..
$ ninja all

```

To automate building and running tests you can use the scripts provided
in the scripts directory, they are pretty simplistic and you can modify
them as you  wish.

## License

The source code is provided as is under the [MIT License](LICENSE)


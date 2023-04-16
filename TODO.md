# TODO

[ ] Create three tasks :
    * "build debug" : Builds a debug version used as pre-task for LLDB Launch.
    * "build release": Builds an optimized build.

[ ] Use CodeLLDB to do runtime debugging for register states
 -> idea: Wrap emulator in DebugContext with forward() and backward() ?
  maybe debug our own emulator in an emulator :monocle: how nice is that.

[x] Add MMU/DRAM support and ensure riscv-tests run.
[ ] Figure out Linking details from Chapter 7 in CS:APP
    * Do we need -fPIC ?

[ ] Figure out LLVM/clang build arguments

[ ] Write an architecture doc and use excalidraw to draw architecture.

[ ] Define a debug mode in `riscvemu::Emulator` to introspect CPU state.

[ ] Check timdbg blogposts, maybe write a time travel debugger (for fun lol)

[ ] Read lldb docs

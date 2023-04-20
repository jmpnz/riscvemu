# TODO

[ ] Write integration test suite :
    [ ] `tests/integration/riscvemu_test.cc` Load and execute bin files
    [ ] `tests/integration/riscv-test-gen.sh` Generate test bin files

[ ] Provide CPU.SetMode(DEBUG) to allow forward, backward stepping
    [ ] Create `struct DebugContext` to keep track of register states.
    [ ] `DebugContext` can store an execution trace.
    [ ] Define a debug mode in `riscvemu::Emulator` to introspect CPU state.

[ ] Ensure correctness with `riscv-tests` for end to end tests. 

[ ] Check timdbg blogposts, maybe write a time travel debugger (for fun lol)


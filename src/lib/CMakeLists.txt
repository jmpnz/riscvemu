set(riscvemu_lib_src
    Instructions.cpp
    Machine.cpp
    )

add_library(libriscvemu ${riscvemu_lib_src})


SET_TARGET_PROPERTIES(libriscvemu PROPERTIES PREFIX "")

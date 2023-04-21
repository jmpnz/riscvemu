#!/bin/bash
if [ $# -ne 1 ]; then
  echo "Usage: $0 file.S"
  exit 1
fi

filename=$(basename -- "$1")
filename="${filename%.*}"
riscv64-unknown-elf-gcc -Wl,-Ttext=0x00 -nostdlib -march=rv64g -mabi=lp64 -o "$filename" "$1"

riscv64-unknown-elf-objcopy -O binary "$filename" "$filename.bin"

rm "$filename"

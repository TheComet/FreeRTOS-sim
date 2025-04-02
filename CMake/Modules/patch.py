import sys
import os

asm = open(sys.argv[1], "rb").read().decode("utf-8")
out = list()
for line in asm.split("\n"):
    out.append(line)
    if line.strip().startswith("mov"):
        out.append("    call instruction_callback_invisible@PLT")
open(sys.argv[1], "wb").write("\n".join(out).encode("utf-8"))


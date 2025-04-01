cmake_minimum_required(VERSION 3.28)
project (foo LANGUAGES C ASM_NASM)
add_executable(foo
    main.c
    portASM.asm)

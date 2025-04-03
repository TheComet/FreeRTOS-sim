cmake_minimum_required (VERSION 3.24)

project (FreeRTOS-Sim
  LANGUAGES C ASM_NASM)

list (APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake/Modules")

add_library (Mobius OBJECT
  "App/Src/Task1.c"
  "App/Src/Task2.c")
target_include_directories (Mobius
  PRIVATE
    "FreeRTOS/portable/GCC/IA32_userspace"
    "FreeRTOS/include"
    "App/Include")

include (InstrumentInstructionCallback)
instrument_instruction_callback (Mobius Mobius_OBJS)

add_executable (FreeRTOS-Sim
  "FreeRTOS/croutine.c"
  "FreeRTOS/event_groups.c"
  "FreeRTOS/list.c"
  "FreeRTOS/queue.c"
  "FreeRTOS/stream_buffer.c"
  "FreeRTOS/tasks.c"
  "FreeRTOS/timers.c"

  "FreeRTOS/portable/GCC/IA32_userspace/port.c"
  "FreeRTOS/portable/GCC/IA32_userspace/port.asm"

  "FreeRTOS/portable/MemMang/heap_4.c"

  ${Mobius_OBJS}

  "App/Src/instruction_callback.c"
  "App/Src/instruction_callback.asm"
  "App/Src/main.c")
target_include_directories (FreeRTOS-Sim
  PRIVATE
    "FreeRTOS/portable/GCC/IA32_userspace"
    "FreeRTOS/include"
    "App/Include")
set_source_files_properties (
  "FreeRTOS/portable/GCC/IA32_userspace/port.c"
  PROPERTIES COMPILE_FLAGS -mgeneral-regs-only)  # __attribute((no_caller_saved_registers))
target_link_options (FreeRTOS-Sim
  PRIVATE
    -z noexecstack)


cmake_minimum_required (VERSION 3.24)

project (FreeRTOS-Sim
  LANGUAGES C ASM_NASM)

list (APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake/Modules")

add_library (Hardware STATIC
  "Hardware/Src/Interrupts.c"
  "Hardware/Src/Gpio.c")
target_include_directories (Hardware PUBLIC "Hardware/Include")

add_library (FreeRTOS STATIC
  "FreeRTOS/croutine.c"
  "FreeRTOS/event_groups.c"
  "FreeRTOS/list.c"
  "FreeRTOS/queue.c"
  "FreeRTOS/stream_buffer.c"
  "FreeRTOS/tasks.c"
  "FreeRTOS/timers.c"

  "FreeRTOS/portable/GCC/IA32_userspace/port.c"
  "FreeRTOS/portable/GCC/IA32_userspace/port.asm"

  "FreeRTOS/portable/MemMang/heap_4.c")
target_include_directories (FreeRTOS
  PUBLIC
    "FreeRTOS/portable/GCC/IA32_userspace"
    "FreeRTOS/include"
  PRIVATE
    "App/Include")
target_link_libraries (FreeRTOS PUBLIC Hardware)
target_link_options (FreeRTOS PRIVATE -z noexecstack)

add_library (App STATIC
  "App/Src/Init.c"
  "App/Src/Task1.c"
  "App/Src/Task2.c")
target_include_directories (App PUBLIC
  "App/Include")
target_link_libraries (App PUBLIC
  FreeRTOS)

include (InstrumentInstructionCallback)
instrument_instruction_callback (App)

add_executable (Harness
  "Harness/Src/InstructionCallback.c"
  "Harness/Src/InstructionCallback.asm"
  "Harness/Src/main.c")
target_link_libraries (Harness PRIVATE
  FreeRTOS
  Hardware
  App)
target_link_options (Harness PRIVATE -z noexecstack)


cmake_minimum_required (VERSION 3.24)

project (FreeRTOS-Sim
  LANGUAGES C ASM)

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
  "FreeRTOS/portable/GCC/IA32_userspace/port.s"

  "FreeRTOS/portable/MemMang/heap_4.c")
target_include_directories (FreeRTOS
  PUBLIC
    "FreeRTOS/portable/GCC/IA32_userspace"
    "FreeRTOS/include"
  PRIVATE
    "App/Include")
target_link_libraries (FreeRTOS PUBLIC Hardware)

add_library (App STATIC
  "App/Src/Init.c"
  "App/Src/Task1.c"
  "App/Src/Task2.c")
target_include_directories (App PUBLIC
  "App/Include")
target_link_libraries (App PUBLIC
  FreeRTOS)

include (InstrumentInstructionCallback)
instrument_instruction_callback (App "InstructionCallback" 5)

add_executable (Simulation
  "Simulation/Src/InstructionCallback.c"
  "Simulation/Src/Tick_posix.c"
  "Simulation/Src/main.c")
target_include_directories (Simulation PRIVATE
  "Simulation/Include")
target_link_libraries (Simulation PRIVATE
  FreeRTOS
  Hardware
  App)

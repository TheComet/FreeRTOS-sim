cmake_minimum_required (VERSION 3.24)

project (FreeRTOS-Sim
  LANGUAGES C ASM_NASM)

list (APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake/Modules")

add_library (Hardware OBJECT
  "Hardware/Src/Hardware.c"
  "Hardware/Include/Hardware/Hardware.h")
target_include_directories (Hardware PUBLIC "Hardware/Include")

add_library (FreeRTOS OBJECT
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

add_library (App OBJECT
  "App/Src/Init.c"
  "App/Src/Task1.c"
  "App/Src/Task2.c")
target_include_directories (App
  PRIVATE
    "Hardware/Include"
    "FreeRTOS/portable/GCC/IA32_userspace"
    "FreeRTOS/include"
    "App/Include")

include (InstrumentInstructionCallback)
instrument_instruction_callback (App App_INSTRUMENTED_OBJS)

add_executable (Harness
  ${App_INSTRUMENTED_OBJS}
  $<TARGET_OBJECTS:FreeRTOS>
  $<TARGET_OBJECTS:Hardware>
  "Harness/Src/InstructionCallback.c"
  "Harness/Src/InstructionCallback.asm"
  "Harness/Src/main.c")
target_include_directories (Harness
  PRIVATE
    $<TARGET_PROPERTY:Hardware,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:App,INCLUDE_DIRECTORIES>)
target_link_options (Harness PRIVATE -z noexecstack)


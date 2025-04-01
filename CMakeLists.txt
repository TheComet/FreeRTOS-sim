cmake_minimum_required (VERSION 3.24)

project (FreeRTOS-Sim
    LANGUAGES C)

add_executable (FreeRTOS-Sim
    "FreeRTOS/croutine.c"
    "FreeRTOS/event_groups.c"
    "FreeRTOS/list.c"
    "FreeRTOS/queue.c"
    "FreeRTOS/stream_buffer.c"
    "FreeRTOS/tasks.c"
    "FreeRTOS/timers.c"

    "App/Src/main.c")
target_include_directories (FreeRTOS-Sim
    PRIVATE
        "FreeRTOS/include"
        "App/Include")


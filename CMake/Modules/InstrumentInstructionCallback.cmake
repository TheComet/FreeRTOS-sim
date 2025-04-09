add_executable (InstrumentInstructionCallback
  ${CMAKE_CURRENT_LIST_DIR}/InstrumentInstructionCallback.c)
set (LAUNCHER 
  ${CMAKE_CURRENT_LIST_DIR}/InstrumentInstructionCallback.sh)

function (instrument_instruction_callback target)
  set_target_properties (${target} PROPERTIES
    C_COMPILER_LAUNCHER ${LAUNCHER})
  add_dependencies (${target} InstrumentInstructionCallback)
endfunction ()


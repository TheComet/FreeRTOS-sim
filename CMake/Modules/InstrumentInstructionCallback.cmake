add_executable (InstrumentInstructionCallback
  ${CMAKE_CURRENT_LIST_DIR}/InstrumentInstructionCallback.c)

function (instrument_instruction_callback target out_objs)
  # This is horrible, is there a better way?
  get_target_property (_copts ${target} COMPILE_OPTIONS)
  get_target_property (_cdefs ${target} COMPILE_DEFINITIONS)
  get_target_property (_inc ${target} INCLUDE_DIRECTORIES)
  set (_cflags)
  foreach (item IN LISTS CMAKE_C_FLAGS)
    list (APPEND _cflags "${item}")
  endforeach ()
  if (${_copts})
    foreach (item IN LISTS _copts)
      list (APPEND _cflags "${item}")
    endforeach ()
  endif ()
  if (_cdefs)
    foreach (item IN LISTS _cdefs)
      list (APPEND _cflags "-D${item}")
    endforeach ()
  endif ()
  if (_inc)
    foreach (item IN LISTS _inc)
      list (APPEND _cflags "-I${item}")
    endforeach ()
  endif ()

  get_target_property (_sources ${target} SOURCES)
  foreach (_source IN LISTS _sources)
    get_filename_component (_filename ${_source} NAME)
    set (_source "${PROJECT_SOURCE_DIR}/${_source}")
    set (_outdir "${PROJECT_BINARY_DIR}/instrumented")
    set (_outfile "${_outdir}/${_filename}")
    add_custom_command (
      DEPENDS ${_source} InstrumentInstructionCallback
      OUTPUT ${_outfile}.o
      COMMAND ${CMAKE_COMMAND} -E make_directory ${_outdir}
      COMMAND ${CMAKE_C_COMPILER} ${_cflags} -S -o- ${_source}
        | $<TARGET_FILE:InstrumentInstructionCallback>
        | ${CMAKE_C_COMPILER} ${_cflags} -o ${_outfile}.o -xassembler -c -
      COMMENT "Instrumenting instruction callbacks for ${_source}"
      VERBATIM)
    list (APPEND ${out_objs} ${_outfile}.o)
  endforeach ()
  set (${out_objs} ${${out_objs}} PARENT_SCOPE)
endfunction ()


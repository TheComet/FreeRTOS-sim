function (patch_instruction_callback target out_objs)
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
    set (_outdir "${PROJECT_BINARY_DIR}/patched")
    set (_outfile "${_outdir}/${_filename}")
    set (_script "${CMAKE_CURRENT_LIST_DIR}/CMake/Modules/patch.py")
    add_custom_command (
      DEPENDS ${_source} ${_script}
      OUTPUT ${_outfile}.s
      COMMAND ${CMAKE_COMMAND} -E make_directory ${_outdir}
      COMMAND ${CMAKE_C_COMPILER} ${_cflags} -S ${_source} -o ${_outfile}.s
      COMMAND python3 ${_script} ${_outfile}.s
      COMMENT "Patching instruction callbacks into ${_source}"
      VERBATIM)
    add_custom_command (
      DEPENDS ${_outfile}.s
      OUTPUT ${_outfile}.o
      COMMAND ${CMAKE_C_COMPILER} ${_cflags} -c ${_outfile}.s -o ${_outfile}.o
      COMMENT "Assembling ${_outfile}.s"
      VERBATIM)
    list (APPEND ${out_objs} ${_outfile}.o)
  endforeach ()
  set (${out_objs} ${${out_objs}} PARENT_SCOPE)
endfunction ()


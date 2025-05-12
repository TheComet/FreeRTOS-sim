find_program (NATIVE_CPP g++ REQUIRED)
execute_process (
  COMMAND ${NATIVE_CPP} -print-file-name=plugin
  OUTPUT_VARIABLE GCC_PLUGIN_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE)

set (INSTRUCTION_CALLBACK_PLUGIN
  "${CMAKE_CURRENT_BINARY_DIR}/instructioncallbackpass.so")
add_custom_command (
  OUTPUT ${INSTRUCTION_CALLBACK_PLUGIN}
  COMMAND ${NATIVE_CPP} -fPIC -fno-rtti -shared
    -I ${GCC_PLUGIN_DIR}/include
    -o ${INSTRUCTION_CALLBACK_PLUGIN}
    ${CMAKE_CURRENT_LIST_DIR}/InstructionCallbackPass.cpp
  DEPENDS ${CMAKE_CURRENT_LIST_DIR}/InstructionCallbackPass.cpp
  COMMENT "Building custom GCC plugin: InstructionCallbackPass")

function (instrument_instruction_callback target callback_name callback_frequency)
  target_compile_options (${target}
    PRIVATE
      -fplugin=${INSTRUCTION_CALLBACK_PLUGIN}
      -fplugin-arg-instructioncallbackpass-callback=${callback_name}
      -fplugin-arg-instructioncallbackpass-frequency=${callback_frequency})
  get_target_property (_sources ${target} SOURCES)
  foreach (src IN LISTS _sources)
    set_source_files_properties (${src} PROPERTIES
      OBJECT_DEPENDS ${INSTRUCTION_CALLBACK_PLUGIN})
  endforeach ()
endfunction ()


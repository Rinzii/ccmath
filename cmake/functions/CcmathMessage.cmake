macro(ccmath_message_color NAME)
  ccmath_message(COLOR ${NAME} "     ${NAME}")
endmacro()

function(internal_ccmath_colored_text)
  cmake_parse_arguments(PARSE_ARGV 0 "_TEXT" "BOLD" "COLOR" "")

  set(_TEXT_OPTIONS -E cmake_echo_color --no-newline)

  if (_TEXT_COLOR)
    string(TOLOWER "${_TEXT_COLOR}" _TEXT_COLOR_LOWER)
    if (_TEXT_COLOR_LOWER STREQUAL "warning")
      set(_TEXT_COLOR_LOWER "yellow")
    endif ()
    if (NOT ${_TEXT_COLOR_LOWER} MATCHES "^default|black|red|green|yellow|warning|blue|magenta|cyan|white")
      ccmath_message("Only these colours are supported:")
      ccmath_message_color(DEFAULT)
      ccmath_message_color(BLACK)
      ccmath_message_color(RED)
      ccmath_message_color(GREEN)
      ccmath_message_color(YELLOW)
      ccmath_message_color(WARNING)
      ccmath_message_color(BLUE)
      ccmath_message_color(MAGENTA)
      ccmath_message_color(CYAN)
      ccmath_message_color(WHITE)
      TEXT(WARING "Color ${_TEXT_COLOR} is not support.")
    else ()
      list(APPEND _TEXT_OPTIONS --${_TEXT_COLOR_LOWER})
    endif ()
  endif ()

  if (_TEXT_BOLD)
    list(APPEND _TEXT_OPTIONS --bold)
  endif ()

  execute_process(COMMAND ${CMAKE_COMMAND} -E env CLICOLOR_FORCE=1 ${CMAKE_COMMAND} ${_TEXT_OPTIONS} "-- " ${_TEXT_UNPARSED_ARGUMENTS}
          OUTPUT_VARIABLE _TEXT_RESULT
          ECHO_ERROR_VARIABLE
  )

  set(TEXT_RESULT ${_TEXT_RESULT} PARENT_SCOPE)
endfunction()
unset(ccmath_message_color)

function(internal_ccmath_normal_text)
  cmake_parse_arguments(PARSE_ARGV 0 "_TEXT" "BOLD" "COLOR" "")
  set(TEXT_RESULT ${_TEXT_UNPARSED_ARGUMENTS} PARENT_SCOPE)
endfunction()

function(ccmath_message)
  if (DEFINED CCMATH_CMAKE_DEBUG AND CCMATH_CMAKE_DEBUG STREQUAL "ON")
    internal_ccmath_colored_text(${ARGN})
    message(${TEXT_RESULT})
  else ()
    internal_ccmath_normal_text(${ARGN})
    message(STATUS ${TEXT_RESULT})
  endif ()
endfunction()

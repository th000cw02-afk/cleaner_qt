if(NOT DEFINED EXE OR NOT DEFINED FIXTURE OR NOT DEFINED OUT)
    message(FATAL_ERROR "run_cli_smoke.cmake requires EXE, FIXTURE, OUT")
endif()

execute_process(
    COMMAND "${EXE}" --scan "${FIXTURE}" --format csv --output "${OUT}"
    RESULT_VARIABLE exit_code
)
if(NOT exit_code EQUAL 0)
    message(FATAL_ERROR "CleanerQt CLI exited with ${exit_code}")
endif()
if(NOT EXISTS "${OUT}")
    message(FATAL_ERROR "CLI did not create output: ${OUT}")
endif()
file(SIZE "${OUT}" out_size)
if(out_size LESS 20)
    message(FATAL_ERROR "CLI output too small (${out_size} bytes): ${OUT}")
endif()

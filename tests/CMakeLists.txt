set(UNITY_DIR ${CMAKE_SOURCE_DIR}/vendor/unity/src)

file(GLOB TEST_SOURCES "test*.c")

add_executable(
  ${PROJECT_NAME}_tests ${TEST_SOURCES} ${CMAKE_SOURCE_DIR}/src/utils/utils.c
                        ${UNITY_DIR}/unity.c $<TARGET_OBJECTS:gta_vi>)

target_link_libraries(${PROJECT_NAME}_tests
                      PRIVATE stb gta_vi ${SDL2_LIBRARIES} ${MATH_LIBRARY})

target_include_directories(
  ${PROJECT_NAME}_tests PRIVATE ${CMAKE_SOURCE_DIR}/src ${UNITY_DIR}
                                ${SDL2_INCLUDE_DIRS})

target_include_directories(
  ${PROJECT_NAME}_tests SYSTEM
  PRIVATE ${CMAKE_SOURCE_DIR}/vendor/GTA-VI/include
          ${CMAKE_SOURCE_DIR}/vendor/GTA-VI/src ${CMAKE_SOURCE_DIR}/vendor/stb)

target_compile_options(${PROJECT_NAME}_tests PRIVATE ${SRC_COMPILE_OPTIONS})

if(ENABLE_COVERAGE)
  target_compile_options(${PROJECT_NAME}_tests PRIVATE -fprofile-arcs
                                                       -ftest-coverage)
endif()

set_target_properties(
  ${PROJECT_NAME}_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                   ${CMAKE_BINARY_DIR}/bin/tests)

add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)

if(ENABLE_COVERAGE)
  find_program(LCOV lcov)
  find_program(GENHTML genhtml)

  if(LCOV AND GENHTML)
    message(STATUS "Found lcov: ${LCOV}")
    message(STATUS "Found genhtml: ${GENHTML}")

    add_custom_target(
      coverage
      COMMAND ${LCOV} --directory . --zerocounters --quiet 2>/dev/null
      COMMAND $<TARGET_FILE:${PROJECT_NAME}_tests>
      COMMAND ${LCOV} --capture --directory . --output-file
              ${CMAKE_BINARY_DIR}/coverage.info --quiet 2>/dev/null
      COMMAND
        ${LCOV} --remove ${CMAKE_BINARY_DIR}/coverage.info '*/tests/*'
        '*/vendor/*' '*/usr/*' '*/unity/*' '*/CMakeFiles/*' --output-file
        ${CMAKE_BINARY_DIR}/coverage.filtered.info --quiet 2>/dev/null
      COMMAND
        ${GENHTML} ${CMAKE_BINARY_DIR}/coverage.filtered.info --output-directory
        ${CMAKE_BINARY_DIR}/coverage_report --quiet 2>/dev/null
      COMMAND ${LCOV} --summary ${CMAKE_BINARY_DIR}/coverage.filtered.info
      COMMAND
        echo
        "HTML coverage report: ${CMAKE_BINARY_DIR}/coverage_report/index.html"
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      DEPENDS ${PROJECT_NAME}_tests
      COMMENT "Generating LCOV code coverage report")

    add_custom_target(
      coverage-summary
      COMMAND ${LCOV} --directory . --zerocounters --quiet 2>/dev/null
      COMMAND $<TARGET_FILE:${PROJECT_NAME}_tests>
      COMMAND ${LCOV} --capture --directory . --output-file
              ${CMAKE_BINARY_DIR}/coverage.info --quiet 2>/dev/null
      COMMAND
        ${LCOV} --remove ${CMAKE_BINARY_DIR}/coverage.info '*/tests/*'
        '*/vendor/*' --output-file ${CMAKE_BINARY_DIR}/coverage.filtered.info
        --quiet 2>/dev/null
      COMMAND ${LCOV} --summary ${CMAKE_BINARY_DIR}/coverage.filtered.info
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      DEPENDS ${PROJECT_NAME}_tests
      COMMENT "Generating code coverage summary")

    add_custom_target(
      coverage-clean
      COMMAND ${LCOV} --directory . --zerocounters
      COMMAND rm -f ${CMAKE_BINARY_DIR}/*.info
      COMMAND rm -rf ${CMAKE_BINARY_DIR}/coverage_report
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMENT "Cleaning coverage data")

  else()
    message(
      WARNING
        "LCOV or genhtml not found. Coverage targets will not be available.")
    if(NOT LCOV)
      message(WARNING "Install lcov: sudo apt-get install lcov")
    endif()
    if(NOT GENHTML)
      message(WARNING "genhtml is part of lcov package")
    endif()
  endif()
endif()

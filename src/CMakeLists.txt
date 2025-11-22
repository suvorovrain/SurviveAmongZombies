file(GLOB_RECURSE SRC_FILES "*.c")

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_compile_options(${PROJECT_NAME} PRIVATE -O2 -Wall -Wextra -pedantic
                                               -Werror)

if(PLATFORM_WINDOWS)
  target_compile_definitions(${PROJECT_NAME} PRIVATE OS_WINDOWS)
elseif(PLATFORM_LINUX)
  target_compile_definitions(${PROJECT_NAME} PRIVATE OS_LINUX)
elseif(PLATFORM_MACOS)
  target_compile_definitions(${PROJECT_NAME} PRIVATE OS_MACOS)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                                 ${CMAKE_BINARY_DIR}/bin)

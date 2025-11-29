file(GLOB_RECURSE SRC_FILES "*.c")

add_executable(${PROJECT_NAME} ${SRC_FILES} $<TARGET_OBJECTS:gta_vi>)

target_link_libraries(${PROJECT_NAME} PRIVATE stb gta_vi ${SDL2_LIBRARIES}
                                              ${MATH_LIBRARY})

target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
target_include_directories(
  ${PROJECT_NAME} SYSTEM
  PRIVATE ${CMAKE_SOURCE_DIR}/vendor/GTA-VI/include
          ${CMAKE_SOURCE_DIR}/vendor/GTA-VI/src ${CMAKE_SOURCE_DIR}/vendor/stb)

target_compile_options(${PROJECT_NAME} PRIVATE ${OUR_COMPILE_OPTIONS})

target_compile_definitions(${PROJECT_NAME} PRIVATE ${SDL2_CFLAGS_OTHER})

if(PLATFORM_WINDOWS)
  target_compile_definitions(${PROJECT_NAME} PRIVATE OS_WINDOWS)
elseif(PLATFORM_LINUX)
  target_compile_definitions(${PROJECT_NAME} PRIVATE OS_LINUX)
elseif(PLATFORM_MACOS)
  target_compile_definitions(${PROJECT_NAME} PRIVATE OS_MACOS)
endif()

target_link_options(${PROJECT_NAME} PRIVATE ${SDL2_LDFLAGS})

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                                 ${CMAKE_BINARY_DIR}/bin)

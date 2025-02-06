cmake_minimum_required(VERSION 3.25 FATAL_ERROR)
project(FirstProjectInCMake)

# Include headers project
include_directories(${UNITY_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/memory/include)

set(SRC_FILES 

    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/memory/src/memory.c
)

# Collect tests
file(GLOB TESTS_FILES ${CMAKE_CURRENT_SOURCE_DIR} unit/*.c)

# Collect library and source files, excluding cJSON.c
file(GLOB SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../src/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/../lib/memory/src/memory.c")
message("SRC_FILES: ${SRC_FILES}")

# Coverage (Optional)
if(RUN_COVERAGE)
    message("Coverage enabled")
    find_program(LCOV_PATH lcov)
        if(NOT LCOV_PATH)
        message(FATAL_ERROR "lcov not found! Aborting...")
    endif()

    find_program(GCOV_PATH gcov)
        if(NOT GCOV_PATH)
        message(FATAL_ERROR "gcov not found! Aborting...")
    endif()

    find_program(GENHTML_PATH genhtml)
        if(NOT GENHTML_PATH)
        message(FATAL_ERROR "genhtml not found! Aborting...")
    endif()

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fprofile-arcs -ftest-coverage --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov --coverage")
endif()

# Create the main project library
add_library(FirstProjectInCMake ${SRC_FILES})


# Create test executable
add_executable(test_FirstProjectInCMake ${TESTS_FILES} ${LIBSTA_SRC_FILES} ${LIBDYN_SRC_FILES})

# Link with Unity and project libraries
target_link_libraries(test_FirstProjectInCMake PRIVATE unity::unity FirstProjectInCMake)

# Add test
add_test(NAME test_${PROJECT_NAME} COMMAND test_${PROJECT_NAME})

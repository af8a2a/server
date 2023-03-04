cmake_minimum_required(VERSION 3.22)
project (Server)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wextra -std=c++20 -pthread")
set(PINE_BUILD_SUPPORT_DIR "${CMAKE_SOURCE_DIR}/build_support")



set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE debug)
set(SERVER_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)
include_directories(${SERVER_SRC_INCLUDE_DIR})
add_subdirectory(${PROJECT_SOURCE_DIR}/src/lib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

# clang-tidy
if (NOT DEFINED CLANG_TIDY_BIN)
    # attempt to find the binary if user did not specify
    find_program(CLANG_TIDY_BIN
            NAMES clang-tidy clang-fidy-8
            HINTS ${PINE_CLANG_SEARCH_PATH})
endif ()
if ("${CLANG_TIDY_BIN}" STREQUAL "CLANG_TIDY_BIN-NOTFOUND")
    message(WARNING "Pine/main couldn't find clang-tidy.")
else ()
    # Output compile_commands.json
    set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
    message(STATUS "Pine/main found clang-fidy at ${CLANG_TIDY_BIN}")
endif ()

# clang-format
if (NOT DEFINED CLANG_FORMAT_BIN)
    # attempt to find the binary if user did not specify
    find_program(CLANG_FORMAT_BIN
            NAMES clang-format clang-format-8
            HINTS ${PINE_CLANG_SEARCH_PATH})
endif ()
if ("${CLANG_FORMAT_BIN}" STREQUAL "CLANG_FORMAT_BIN-NOTFOUND")
    message(WARNING "Pine/main couldn't find clang-format.")
else ()
    message(STATUS "Pine/main found clang-format at ${CLANG_FORMAT_BIN}")
endif ()






add_executable(server src/server.cpp)
add_executable(client src/client.cpp)
add_executable(test src/test.cc)
target_link_libraries(server LIB)
target_link_libraries(client LIB)
target_link_libraries(test LIB)




###########################################################
# "make clang-tidy" target
###########################################################
# runs clang-tidy and exits with a non-zero exit code if any errors are found.
# note that clang-tidy automatically looks for a .clang-tidy file in parent directories
add_custom_target(clang-tidy
        ${PINE_BUILD_SUPPORT_DIR}/run_clang_tidy.py                     # run LLVM's clang-tidy script
        -clang-tidy-binary ${CLANG_TIDY_BIN}                              # using our clang-tidy binary
        -p ${CMAKE_BINARY_DIR}                                            # using cmake's generated compile commands
        )

        ##########################################
# "make format"
# "make check-format"
##########################################
string(CONCAT PINE_FORMAT_DIRS
"${CMAKE_CURRENT_SOURCE_DIR}/src,"
"${CMAKE_CURRENT_SOURCE_DIR}/test,"
)

# runs clang format and updates files in place.
add_custom_target(format ${PINE_BUILD_SUPPORT_DIR}/run_clang_format.py
${CLANG_FORMAT_BIN}
${PINE_BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
--source_dirs
${PINE_FORMAT_DIRS}
--fix
--quiet
)

# runs clang format and exits with a non-zero exit code if any files need to be reformatted
add_custom_target(check-format ${PINE_BUILD_SUPPORT_DIR}/run_clang_format.py
${CLANG_FORMAT_BIN}
${PINE_BUILD_SUPPORT_DIR}/clang_format_exclusions.txt
--source_dirs
${PINE_FORMAT_DIRS}
--quiet
)
cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)

project(reflect LANGUAGES CXX VERSION 0.1)

set(libsrc
    src/detail/accessor.cpp
    src/type.cpp
)

set(testsrc
    tests/common/main.cpp
    tests/conversion.cpp
    tests/object_access.cpp
    tests/object_construct.cpp
)

include_directories(include)

# Compiler warnings.
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-unused-parameter")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif()

# Object files.
add_library(object-files OBJECT ${libsrc})
set_property(TARGET object-files PROPERTY POSITION_INDEPENDENT_CODE ON)

# Shared library target.
add_library(reflect-shared SHARED $<TARGET_OBJECTS:object-files>)
set_target_properties(reflect-shared
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY lib/shared
    LIBRARY_OUTPUT_DIRECTORY lib/shared
    RUNTIME_OUTPUT_DIRECTORY lib/shared
    OUTPUT_NAME reflect
)

add_custom_target(shared DEPENDS reflect-shared)

# Static library target.
add_library(reflect-static STATIC $<TARGET_OBJECTS:object-files>)
set_target_properties(reflect-static
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY lib/static
    OUTPUT_NAME reflect
)

add_custom_target(static DEPENDS reflect-static)

# Check target.
add_custom_target(check COMMAND ./unit-tests)

add_executable(unit-tests EXCLUDE_FROM_ALL ${testsrc})
target_link_libraries(unit-tests reflect-static)
add_test(unit-tests unit-tests)
add_dependencies(check unit-tests)

# Code coverage.
if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

# Set the minimum version of cmake required to build
cmake_minimum_required(VERSION 3.22.1)

# Set module path  
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")
message(STATUS "CMAKE_MODULE_PATH has been set as: ${CMAKE_MODULE_PATH}")

# Set up compiler to be used
include(SetupCompiler)

# Define project
project(Bolt VERSION 0.0.0)

# Set project build type
IF (NOT DEFINED ${CMAKE_BUILD_TYPE})
    set (CMAKE_BUILD_TYPE "DEBUG" CACHE STRING "Specify build type." FORCE)
    message (STATUS "Set CMAKE_BUILD_TYPE to default of: ${CMAKE_BUILD_TYPE}")
ENDIF()
message(STATUS "CMAKE_BUILD_TYPE has been set as: ${CMAKE_BUILD_TYPE}")

# Global compiler flags
include(CompilerFlags)

# START - add any third party libraries

# Boost
set(Boost_USE_STATIC_LIBS TRUE)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.74 COMPONENTS system filesystem unit_test_framework program_options REQUIRED)
message(STATUS "Boost version: ${Boost_VERSION}")

# END - add any third party libraries

# Set include directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/core)

# Setup CTest
include(CTest)
enable_testing()

# Add modules
add_subdirectory(apps)
add_subdirectory(multiproc)

install(DIRECTORY "cmake/modules" DESTINATION "cmake")
install(FILES "${CMAKE_BINARY_DIR}/CMakeCache.txt" DESTINATION "build")
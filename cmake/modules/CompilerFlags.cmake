# Set compiler flags

# Set C and C++ versions
set(C_VERSION "-std=c99")
set(CXX_VERSION "-std=c++20")

# Set global compiler flags
set(GLOBAL_COMPILER_FLAGS "${GLOBAL_COMPILER_FLAGS} ${CXX_VERSION} -lrt -m64 -Wall -pthread -fPIC -Wno-terminate")

# Set flags for debug / release builds
set(CMAKE_CXX_FLAGS_DEBUG "-D_GLIBCXX_ASSERTIONS -ggdb -DDEBUG -Werror -pipe ${GLOBAL_COMPILER_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -pipe ${GLOBAL_COMPILER_FLAGS}")

message(STATUS "Debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "Release flags: ${CMAKE_CXX_FLAGS_RELEASE}")

# Set C flags
set(CMAKE_C_FLAGS "${C_VERSION}")
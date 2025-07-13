# Create a static library from the multiproc source
add_library(multiproc STATIC
    RingBuffer.cpp
)

# Specify the include directory for consumers of the library
target_include_directories(multiproc
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

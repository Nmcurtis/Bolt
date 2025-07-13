set(APPNAME shm_ping)

add_executable(${APPNAME} main.cpp)
target_link_libraries(${APPNAME} PRIVATE multiproc)

install(TARGETS ${APPNAME} DESTINATION bin)
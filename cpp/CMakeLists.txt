set(TARGET sock)

# compile target
if(WIN32)
  add_library(${TARGET} 
    src/windows/socket_base.cpp
    src/windows/tcp_socket.cpp
    src/windows/udp_socket.cpp)
  target_link_libraries(${TARGET} ws2_32.lib)
else()
  add_library(${TARGET} 
    src/unix/socket_base.cpp
    src/unix/tcp_socket.cpp
    src/unix/udp_socket.cpp)
endif()

# additional include directories
target_include_directories(${TARGET} PRIVATE include)

# output directory (binary)
install(TARGETS ${TARGET}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION bin
  RUNTIME DESTINATION bin
)

# output directory (header)
install(DIRECTORY include
  DESTINATION include
)

# test
add_subdirectory(tests)

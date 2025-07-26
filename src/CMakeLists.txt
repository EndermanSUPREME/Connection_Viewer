file(GLOB CONN_SRC cpp/*.cpp)
file(GLOB CONN_HEADER headers/*.hpp)

add_executable(connvwr ${CONN_SRC} ${CONN_HEADER})
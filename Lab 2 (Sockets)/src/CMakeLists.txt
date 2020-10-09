add_library (MultithreadedServer 
                MultithreadedServer.cpp 
                MultithreadedServer.hpp)

add_library(dnsresolver dnsresolver.cpp)

add_executable (web-server web-server.cpp)

target_link_libraries (web-server MultithreadedServer)

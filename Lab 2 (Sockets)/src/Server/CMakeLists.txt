add_library (MultithreadedServer 
                MultithreadedServer.cpp 
                MultithreadedServer.hpp)
target_link_libraries (MultithreadedServer HTTPResponse)


add_executable (web-server web-server.cpp)

target_link_libraries (web-server MultithreadedServer)

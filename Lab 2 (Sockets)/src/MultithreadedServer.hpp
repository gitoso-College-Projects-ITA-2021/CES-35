#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include <iostream>
#include <sstream>
#include <thread>


#define BUFFER_SIZE 1024

class MultithreadedServer {
public:
    MultithreadedServer(const std::string& host, int port, const std::string& ip, const std::string& directory);
    void AcceptConnections();

private:
    void ServerJob(int jobId, int clientSockfd);

    const int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    const std::string& ip;
    const std::string& dir;
};

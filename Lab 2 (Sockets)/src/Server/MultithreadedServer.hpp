#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

#define BUFFER_SIZE 32768  // 32Kbytes

class MultithreadedServer {
 public:
  MultithreadedServer(const std::string& host, int port, const std::string& ip,
                      const std::string& directory);
  void AcceptConnections();

 private:
  void ServerJob(int jobId, int clientSockfd);

  const int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  const std::string& ip;
  const std::string& dir;
};

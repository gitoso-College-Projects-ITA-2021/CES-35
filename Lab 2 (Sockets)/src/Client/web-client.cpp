#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include "../Lib/HTTPRequest.hpp"


int main(int argc, char *argv[]) {

 for(int i = 1; i < argc; ++i ){
    HTTPRequest request;

    // Cria a URL
    request.setURL(argv[i]);

    // cria o socket TCP IP
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // =====================================================================================
    struct addrinfo hints;
    struct addrinfo* res;
    char ipstr[INET_ADDRSTRLEN] = {'\0'};

    // hints - modo de configurar o socket para o tipo  de transporte
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
     // funcao de obtencao do endereco via DNS - getaddrinfo 
    // funcao preenche o buffer "res" e obtem o codigo de resposta "status" 
    int status = 0;
    if ((status = getaddrinfo(request.hostname, "80", &hints, &res)) != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
        return 2;
    }

    for(struct addrinfo* p = res; p != 0; p = p->ai_next) {
        // a estrutura de dados eh generica e portanto precisa de type cast
        struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;

        // e depois eh preciso realizar a conversao do endereco IP para string
        inet_ntop(p->ai_family, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
    }

    freeaddrinfo(res); // libera a memoria alocada dinamicamente para "res"
    // =====================================================================================

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(request.port);     // short, network byte order
    serverAddr.sin_addr.s_addr = inet_addr(ipstr);
    memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

    // conecta com o servidor atraves do socket
    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
      perror("connect");
      return 2;
    }

    // a partir do SO, eh possivel obter o endereço IP usando 
    // pelo cliente (nos mesmos) usando a funcao getsockname
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    if (getsockname(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen) == -1) {
      perror("getsockname");
      return 3;
    }

    char buf[BUFFER_SIZE] = {0};
    char buf2[BUFFER_SIZE] = {0};

    // send request
    request.setMethod("GET");
    std::string input = request.buildRequest();
    std::cout << "sending: " << std::endl;
    std::cout << input << std::endl;
    if (send(sockfd, input.c_str(), input.size(), 0) == -1) {
      perror("send");
      return 4;
    }

    // zera o buffer
    memset(buf, '\0', sizeof(buf));

    // recebe no buffer uma certa quantidade de bytes BUFFER_SIZE
    int received = recv(sockfd, buf, BUFFER_SIZE, 0);
    if (received == -1) {
      perror("recv");
      return 5;
    }

    int responseStatus = request.getResponseStatus(buf);
    switch (responseStatus)
    {
        case 200:
        {
            int conntentLenght = request.getContentLenght(buf);

            // open file
            std::ofstream outfile;
            outfile.open(request.getObject(), std::ios_base::out | std::ios_base::binary);

            // handle first part of request.
            request.handle(buf2, buf, received);

            // write on file
            outfile.write(buf2, received);

            //std::cout << "\nReceived: (" << received << "/" << conntentLenght << ")" << std::endl; 
            while(received < conntentLenght){
                // zera o buffer
                memset(buf2, '\0', sizeof(buf2));

                // recebe no buffer uma certa quantidade de bytes ate BUFFER_SIZE
                int r = recv(sockfd, buf2, BUFFER_SIZE, 0);
                if (r == -1) {
                    perror("recv");
                    return 5;
                }
                received += r;
                outfile.write(buf2, r);
                //std::cout << "\nReceived: (" << received << "/" << conntentLenght << ")"; 
            }
            // close file
            outfile.close();
            break;
        }
        default:
            // print response
            printf("%s\n", buf);
            break;
    }
    
    // fecha o socket
    close(sockfd);

  }
  return 0;
}

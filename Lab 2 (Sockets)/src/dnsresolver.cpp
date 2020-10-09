#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <unistd.h>
#include <netdb.h>
#include <iostream>
#include <sstream>

const std::string dns_resolver (char* host) {
    struct addrinfo hints;
    struct addrinfo* res;

    // salvando a resposta nesta variavel
    std::string ip;

    // printando o input
    std::cout << "Resolving dns for: " << host << std::endl;

    // hints - modo de configurar o socket para o tipo  de transporte
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    // funcao de obtencao do endereco via DNS - getaddrinfo
    // funcao preenche o buffer "res" e obtem o codigo de resposta "status"
    int status = 0;
    if ((status = getaddrinfo(host, "80", &hints, &res)) != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
        exit(2);
    }

    // printando as respostas
    std::cout << "  => Answers: [" << std::endl;

    for(struct addrinfo* p = res; p != 0; p = p->ai_next) {
        // a estrutura de dados eh generica e portanto precisa de type cast
        struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;

        // e depois eh preciso realizar a conversao do endereco IP para string
        char ipstr[INET_ADDRSTRLEN] = {'\0'};
        inet_ntop(p->ai_family, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
        std::cout << "       " << ipstr << std::endl;

        // salvando uma das resposta
        ip = ipstr;
    }
    std::cout << "  => ]" << std::endl;

    freeaddrinfo(res); // libera a memoria alocada dinamicamente para "res"

    std::cout << "  => Chosen ip: " << ip << std::endl << std::endl;
    return ip;
}

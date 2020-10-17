/* =====================================================================================
(CES-35): Laboratório II - Implementação de Sockets

Grupo:
- Adrisson Rogério Samersla
- Gianluigi Dal Toso
- Lucas Alberto Bilobran Lema
----------------------------------------------------------------------------------------
TODO:
    - [ ] Server Multithreaded
    - [ ] Quebrar o arquivo em chunks para o envio se for muito grande
    - [ ] Tratar e devolver códigos de erro
    - [ ] Impedir PATH Traversal
    - [ ] Setar os tamanhos dos buffers e arrays
    - [x] Na requisição do '/' devolver `index.html` por padrão
======================================================================================*/

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <sstream>

#include "../Lib/dnsresolver.cpp"
#include "./MultithreadedServer.hpp"

void print_header(char *, char *, char *, char *);

int main(int argc, char *argv[]) {
  // [x] TODO: Processar os argumentos da main
  // ===================================================
  char *arg_host;
  char default_arg_host[10] = "localhost";
  char *arg_port;
  char default_arg_port[5] = "3000";
  char *arg_dir;
  char default_arg_dir[5] = "/tmp";

  // Se uso incorreto ou usuário pedindo ajuda mostrar mensagem de "usage"
  if (argc > 4 || (argc > 1 && (strcmp(argv[1], "-help") == 0 ||
                                strcmp(argv[1], "--help") == 0 ||
                                strcmp(argv[1], "-h") == 0))) {
    std::cerr << "Usage: web-server [host] [port] [dir]\n\n" << std::endl;
    return 1;
  } else {
    arg_host = (argc > 1 ? argv[1] : default_arg_host);
    arg_port = (argc > 2 ? argv[2] : default_arg_port);
    arg_dir = (argc > 3 ? argv[3] : default_arg_dir);
  }

  // Resolving DNS
  std::string ipstr = dns_resolver(arg_host);

  // Convert port string to integer
  std::stringstream stream_port(arg_port);
  int port = 0;
  stream_port >> port;

  std::string host(arg_host);
  std::string dir(arg_dir);

  MultithreadedServer server(host, port, ipstr, dir);
  server.AcceptConnections();
}

// int main(int argc, char *argv[]) {
//     // [x] TODO: Processar os argumentos da main
//     =================================================== char *arg_host; char
//     default_arg_host[10] = "localhost"; char *arg_port; char
//     default_arg_port[5] = "3000"; char *arg_dir; char default_arg_dir[5] =
//     "/tmp";

//     // Se uso incorreto ou usuário pedindo ajuda mostrar mensagem de "usage"
//     if (argc > 4 ||
//        (argc > 1 && (strcmp(argv[1], "-help")  == 0 ||
//                      strcmp(argv[1], "--help") == 0 ||
//                      strcmp(argv[1], "-h")     == 0 )))
//     {
//         std::cerr << "Usage: web-server [host] [port] [dir]\n\n" <<
//         std::endl; return 1;
//     }
//     else {
//         arg_host = (argc > 1 ? argv[1] : default_arg_host);
//         arg_port = (argc > 2 ? argv[2] : default_arg_port);
//         arg_dir  = (argc > 3 ? argv[3] : default_arg_dir);
//     }

//     // [x] TODO: Resolver o endereço do HOSTNAME (DNS)
//     ============================================ struct addrinfo hints;
//     struct addrinfo* res;
//     char ipstr[INET_ADDRSTRLEN] = {'\0'};

//     // hints - modo de configurar o socket para o tipo  de transporte
//     memset(&hints, 0, sizeof(hints));
//     hints.ai_family = AF_INET; // IPv4
//     hints.ai_socktype = SOCK_STREAM; // TCP

//     // funcao de obtencao do endereco via DNS - getaddrinfo
//     // funcao preenche o buffer "res" e obtem o codigo de resposta "status"
//     int status = 0;
//     if ((status = getaddrinfo(arg_host, "80", &hints, &res)) != 0) {
//         std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
//         return 2;
//     }

//     for(struct addrinfo* p = res; p != 0; p = p->ai_next) {
//         // a estrutura de dados eh generica e portanto precisa de type cast
//         struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;

//         // e depois eh preciso realizar a conversao do endereco IP para
//         string inet_ntop(p->ai_family, &(ipv4->sin_addr), ipstr,
//         sizeof(ipstr));
//     }

//     freeaddrinfo(res); // libera a memoria alocada dinamicamente para "res"

//     // Print Header in StdOut
//     =====================================================================
//     print_header(arg_host, ipstr, arg_port, arg_dir);

//     // Aceita uma solicitação TCP
//     ================================================================
//     // cria um socket para IPv4 e usando protocolo de transporte TCP
//     int sockfd = socket(AF_INET, SOCK_STREAM, 0);

//     // Opções de configuração do SOCKETs
//     // No sistema Unix um socket local TCP fica preso e indisponível
//     // por algum tempo após close, a não ser que configurado SO_REUSEADDR
//     int yes = 1;
//     if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ==
//     -1) {
//         perror("setsockopt");
//         return 1;
//     }

//     // struct sockaddr_in {
//     //  short            sin_family;   // e.g. AF_INET, AF_INET6
//     //  unsigned short   sin_port;     // e.g. htons(3490)
//     //  struct in_addr   sin_addr;     // see struct in_addr, below
//     //  char             sin_zero[8];  // zero this if you want to
//     // };

//     // Convert port string to integer
//     std::stringstream stream_port(arg_port);
//     int port = 0;
//     stream_port >> port;

//     // Connect
//     struct sockaddr_in addr;
//     addr.sin_family = AF_INET;
//     addr.sin_port = htons(port);     // porta tem 16 bits, logo short,
//     network byte order addr.sin_addr.s_addr = inet_addr(ipstr);
//     memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

//     // realizar o bind (registrar a porta para uso com o SO) para o socket
//     if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
//         perror("bind");
//         return 2;
//     }

//     // colocar o socket em modo de escuta, ouvindo a porta
//     if (listen(sockfd, 1) == -1) {
//         perror("listen");
//         return 3;
//     }

//     // aceitar a conexao TCP
//     // verificar que sockfd e clientSockfd sao sockets diferentes
//     // sockfd eh a "socket de boas vindas"
//     // clientSockfd eh a "socket diretamente com o cliente"
//     struct sockaddr_in clientAddr;
//     socklen_t clientAddrSize = sizeof(clientAddr);

//     // Receber e processar a requisição HTTP
//     // COMMENTS: Fazer como se fosse via HTTP 1.0, um socket por objeto
//     =============================== bool isEnd = false; while (!isEnd) {
//         int httpStatusCode = 200; // Será modificado se der algum erro no
//         caminho int clientSockfd = accept(sockfd, (struct
//         sockaddr*)&clientAddr, &clientAddrSize);

//         if (clientSockfd == -1) {
//             perror("accept");
//             return 4;
//         }

//         // usa um vetor de caracteres para preencher o endereço IP do cliente
//         ipstr[INET_ADDRSTRLEN] = {'\0'};

//         // faz leitura e escrita dos dados da conexao
//         // utiliza um buffer de BUFFER_SIZE bytes (char)
//         char buf[BUFFER_SIZE + 1] = {0};

//         // zera a memoria do buffer
//         memset(buf, '\0', sizeof(buf));

//         // recebe ate 1024 bytes do cliente remoto
//         if (recv(clientSockfd, buf, BUFFER_SIZE, 0) == -1) {
//             perror("recv");
//             return 5;
//         }

//         // Imprime o que foi recebido na tela do servidor
//         std::stringstream ss;

//         std::cout << "======= Received Request ==========" << std::endl;
//         ss << buf << std::endl;
//         std::cout << buf << std::endl;

//         // Parse de HTTP
//         ==============================================================================
//         //     HTTP válido:
//         //         GET /[path]/[filename] HTTP/1.0
//         //
//         //     Estamos ignorando quaisquer headers de protocolo por enquanto
//         char filename[200];
//         char completeFilename[300] = "\0";
//         char httpVersion[5];
//         if (sscanf(buf, "GET %s HTTP/%s\n %*s", filename, httpVersion) != 2)
//         {
//             httpStatusCode = 400; // Bad Request
//         } else if (filename[0] != '/') {
//             httpStatusCode = 400; // Bad Request
//         }

//         // [ ] TODO: Impedir path traversal
//         ===============================================================
//         // Tenta localizar o arquivo
//         FILE *localFile;
//         if (httpStatusCode == 200) {
//             strcat(completeFilename, arg_dir);
//             if (strlen(filename) == 1) {
//                 // filename == '/'
//                 strcat(completeFilename, "/index.html");
//             }
//             else {
//                 strcat(completeFilename, filename);
//             }

//             // Procura o arquivo solicitado
//             ===============================================================
//             localFile = fopen(completeFilename, "r");

//             if (localFile == NULL) {
//                 httpStatusCode = 404; // Not Found
//             }
//         }

//         // Código retirado da internet - Fonte:
//         https://stackoverflow.com/questions/2029103/correct-way-to-read-a-text-file-into-a-buffer-in-c
//         // Cria a mensagem de retorno
//         ======================================================================
//         char response[BUFFER_SIZE + 1];
//         char fileContents[200];

//         // Criar uma resposta
//         // [ ] TODO: Quebrar o arquivo em chunks

//         switch (httpStatusCode) {
//             case 404:
//                 sprintf(response, "HTTP/1.0 %d %s\n", httpStatusCode, "Not
//                 Found"); printf("Responding with Header:\n%s", response);
//             break;

//             case 400:
//                 sprintf(response, "HTTP/1.0 %d %s\n", httpStatusCode, "Bad
//                 Request"); printf("Responding with Header:\n%s", response);
//             break;

//             case 200:
//                 // Calcula o tamanho do conteúdo do arquivo e põe no header

//                 /* Seek to the beginning of the file */
//                 fseek(localFile, 0, SEEK_END);
//                 long lSize = ftell(localFile);
//                 rewind(localFile);

//                 /* Read data */
//                 fread(fileContents, 1, lSize, localFile);
//                 fclose(localFile);

//                 //sprintf(response, "HTTP/1.0 %d %s\nContent-Lenght: %d\n\n",
//                 httpStatusCode, "OK", lSize - 1); sprintf(response, "HTTP/1.0
//                 %d %s\n\n", httpStatusCode, "OK"); printf("===== Responding
//                 with Header: =====\n%s", response);

//                 /* Concatenate file contents and Responde headers */
//                 strcat(response, fileContents);
//             break;
//         }

//         // Envia a resposta para o cliente
//         if (send(clientSockfd, response, strlen(response), 0) == -1) {
//             perror("send");
//             return 6;
//         }

//         // fecha o socket
//         close(clientSockfd);
//     }

//     // TAREFAS ESPERADAS:
//     //  [x] a) Converter o nome do host do servidor em endereço IP, abrir
//     socket para escuta neste  endereço IP e no número de porta especificado
//     //  [x] b) Por meio do socket "listen" aceitar solicitações de conexão
//     dos clientes, e estabelecer conexões com os clientes.
//     //  [ ] c) Fazer  uso  de  programação  de  redes  que  lide  com
//     conexões simultâneas (por exemplo, por meio de multiprocess e
//     multithreads). Ou seja, o servidor web deve poder receber solicitações de
//     vários clientes ao mesmo tempo. return 0;
// }

void print_header(char *host, char *translated_host, char *port, char *dir) {
  printf("===== STARTING AWESOME SERVER =====\n\n");
  if (strcmp(host, translated_host) == 0) {
    printf("Host: %s\nPort: %s\nDirectory: %s\n\n", host, port, dir);
  } else {
    printf("Host: %s (%s)\nPort: %s\nDirectory: %s\n\n", host, translated_host,
           port, dir);
  }
  printf("===================================\n\n");
}

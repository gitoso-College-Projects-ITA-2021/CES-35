#include "./MultithreadedServer.hpp"

#include <cstring>

#include "../Lib/HTTPResponse.cpp"

int file_size(std::ifstream& file) {
  int length = -1;

  file.seekg(0, file.end);
  length = file.tellg();
  file.seekg(0, file.beg);

  return length;
}

MultithreadedServer::MultithreadedServer(const std::string& host, int port,
                                         const std::string& ip_str,
                                         const std::string& directory)
    : ip(ip_str), dir(directory) {
  // checa a criacao de um socket para IPv4 e usando protocolo de transporte TCP
  if (this->sockfd == -1) {
    perror("socket");
    exit(1);
  }

  // Opções de configuração do SOCKETs
  // No sistema Unix um socket local TCP fica preso e indisponível
  // por algum tempo após close, a não ser que configurado SO_REUSEADDR
  int yes = 1;
  if (setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ==
      -1) {
    perror("setsockopt");
    exit(1);
  }

  // Connect
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port =
      htons(port);  // porta tem 16 bits, logo short, network byte order
  addr.sin_addr.s_addr = inet_addr(this->ip.c_str());
  memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

  // realizar o bind (registrar a porta para uso com o SO) para o socket
  if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("bind");
    exit(2);
  }

  // colocar o socket em modo de escuta, ouvindo a porta
  if (listen(sockfd, 1) == -1) {
    perror("listen");
    exit(3);
  }
}

void MultithreadedServer::AcceptConnections() {
  // aceitar a conexao TCP
  // verificar que sockfd e clientSockfd sao sockets diferentes
  // sockfd eh a "socket de boas vindas"
  // clientSockfd eh a "socket diretamente com o cliente"
  struct sockaddr_in clientAddr;
  socklen_t clientAddrSize = sizeof(clientAddr);

  // Receber e processar a requisição HTTP
  // COMMENTS: Fazer como se fosse via HTTP 1.0, um socket por objeto
  // ===============================
  bool isEnd = false;
  while (!isEnd) {
    int clientSockfd =
        accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrSize);

    std::cout << "ACEITEI!!!" << std::endl;

    if (clientSockfd == -1) {
      perror("accept");
      exit(4);
    }

    std::thread job(&MultithreadedServer::ServerJob, this, 1, clientSockfd);
    job.detach();
  }
}

void MultithreadedServer::ServerJob(int jobId, int clientSockfd) {
  std::cout << "Job: " << jobId << std::endl;

  HTTP::StatusCode httpStatusCode = HTTP::OK;
  HTTP::Response response;

  // usa um vetor de caracteres para preencher o endereço IP do cliente
  char ipstr[INET_ADDRSTRLEN] = {'\0'};

  // faz leitura e escrita dos dados da conexao
  // utiliza um buffer de BUFFER_SIZE bytes (char)
  char buf[BUFFER_SIZE] = {0};

  // zera a memoria do buffer
  memset(buf, '\0', sizeof(buf));

  // recebe ate 1024 bytes do cliente remoto
  if (recv(clientSockfd, buf, BUFFER_SIZE, 0) == -1) {
    perror("recv");
    exit(5);
  }

  // Imprime o que foi recebido na tela do servidor
  std::stringstream ss;

  std::cout << "======= Received Request ==========" << std::endl;
  ss << buf << std::endl;
  std::cout << buf << std::endl;

  // Parse de HTTP
  //
  // == == == == == == == == == == == == == == == == == == == == == == == == ==
  // == == == == == == == == == == == == == ==
  //     HTTP válido:
  //         GET /[path]/[filename] HTTP/1.0
  //
  //     Estamos ignorando quaisquer headers de protocolo por enquanto
  char filename[200];
  char completeFilename[300] = "\0";
  char httpVersion[5];
  if (sscanf(buf, "GET %s HTTP/%s\n %*s", filename, httpVersion) != 2) {
    httpStatusCode = HTTP::BadRequest;  // Bad Request
  } else if (filename[0] != '/') {
    httpStatusCode = HTTP::BadRequest;  // Bad Request
  }

  // [ ] TODO: Impedir path traversal
  // =============================================================== Tenta
  // localizar o arquivo
  std::ifstream localFile;
  if (httpStatusCode == 200) {
    strcat(completeFilename, this->dir.c_str());
    if (strlen(filename) == 1) {
      // filename == '/'
      strcat(completeFilename, "/index.html");
    } else {
      strcat(completeFilename, filename);
    }

    // Procura o arquivo solicitado
    // ===============================================================
    localFile.open(completeFilename, std::ios_base::in | std::ios_base::binary);

    if (!localFile.good()) {
      httpStatusCode = HTTP::NotFound;  // Not Found
    }
  }

  // Setting response's HTTP Status Code
  response.setStatusCode(httpStatusCode);

  // response's header
  std::string header;
  response.decode(header);

  if (response.getStatusCode() != HTTP::OK) {
    // Não há arquivo: apenas os headers
    if (send(clientSockfd, header.c_str(), strlen(header.c_str()), 0) == -1) {
      perror("send");
      exit(6);
    }
  } else {
    char response_buf[BUFFER_SIZE] = {'\0'};
    int fileLen = file_size(localFile);
    response.setContentLength(fileLen);

    // Preenchendo com o header
    int headerLen = header.copy(response_buf, BUFFER_SIZE, 0) - 1;

    // Preenchendo o resto com o arquivo
    localFile.read(&response_buf[headerLen], BUFFER_SIZE - headerLen);
    int quantRead = localFile.gcount();

    // Envia a resposta para o cliente
    if (send(clientSockfd, response_buf, headerLen + quantRead, 0) == -1) {
      perror("send");
      exit(6);
    }

    int quantSend = quantRead;
    while (quantSend < fileLen) {
      localFile.read(response_buf, BUFFER_SIZE);
      quantRead = localFile.gcount();

      // Envia a resposta para o cliente
      int len = send(clientSockfd, response_buf, quantRead, 0);
      if (len == -1) {
        perror("send");
        exit(6);
      }

      quantSend += len;
    }

    localFile.close();
  }

  // fecha o socket
  close(clientSockfd);
}

// void MultithreadedServer::ServerJob(int jobId, int clientSockfd) {

//   int httpStatusCode = 200;  // Será modificado se der algum erro no
//   caminho

//   std::cout << "Job: " << jobId << std::endl;

//   // usa um vetor de caracteres para preencher o endereço IP do cliente
//   char ipstr[INET_ADDRSTRLEN] = {'\0'};

//   // faz leitura e escrita dos dados da conexao
//   // utiliza um buffer de BUFFER_SIZE bytes (char)
//   char buf[BUFFER_SIZE] = {0};

//   // zera a memoria do buffer
//   memset(buf, '\0', sizeof(buf));

//   // recebe ate 1024 bytes do cliente remoto
//   if (recv(clientSockfd, buf, BUFFER_SIZE, 0) == -1) {
//     perror("recv");
//     exit(5);
//   }

//   // Imprime o que foi recebido na tela do servidor
//   std::stringstream ss;

//   std::cout << "======= Received Request ==========" << std::endl;
//   ss << buf << std::endl;
//   std::cout << buf << std::endl;

//   // Parse de HTTP
//   //
//   ==============================================================================
//   //     HTTP válido:
//   //         GET /[path]/[filename] HTTP/1.0
//   //
//   //     Estamos ignorando quaisquer headers de protocolo por enquanto
//   char filename[200];
//   char completeFilename[300] = "\0";
//   char httpVersion[5];
//   if (sscanf(buf, "GET %s HTTP/%s\n %*s", filename, httpVersion) != 2) {
//     httpStatusCode = 400;  // Bad Request
//   } else if (filename[0] != '/') {
//     httpStatusCode = 400;  // Bad Request
//   }

//   // [ ] TODO: Impedir path traversal
//   // =============================================================== Tenta
//   // localizar o arquivo
//   std::ifstream localFile;
//   if (httpStatusCode == 200) {
//     strcat(completeFilename, this->dir.c_str());
//     if (strlen(filename) == 1) {
//       // filename == '/'
//       strcat(completeFilename, "/index.html");
//     } else {
//       strcat(completeFilename, filename);
//     }

//     // Procura o arquivo solicitado
//     // ===============================================================
//     localFile.open(completeFilename, std::ios_base::in |
//     std::ios_base::binary);

//     if (!localFile.good()) {
//       httpStatusCode = 404;  // Not Found
//     }
//   }

//   // Código retirado da internet - Fonte:
//   //
//   https://stackoverflow.com/questions/2029103/correct-way-to-read-a-text-file-into-a-buffer-in-c
//   // Cria a mensagem de retorno
//   // ======================================================================
//   char response[BUFFER_SIZE];

//   // Criar uma resposta
//   // [ ] TODO: Quebrar o arquivo em chunks

//   switch (httpStatusCode) {
//     case 404:
//       sprintf(response, "HTTP/1.0 %d %s\n", httpStatusCode, "Not Found");
//       printf("Responding with Header:\n%s", response);

//       // Envia a resposta para o cliente
//       if (send(clientSockfd, response, strlen(response), 0) == -1) {
//         perror("send");
//         exit(6);
//       }
//       break;

//     case 400:
//       sprintf(response, "HTTP/1.0 %d %s\n", httpStatusCode, "Bad Request");
//       printf("Responding with Header:\n%s", response);

//       // Envia a resposta para o cliente
//       if (send(clientSockfd, response, strlen(response), 0) == -1) {
//         perror("send");
//         exit(6);
//       }
//       break;

//     case 200:
//       // get length of file:
//       localFile.seekg(0, localFile.end);
//       int length = localFile.tellg();
//       localFile.seekg(0, localFile.beg);

//       sprintf(response, "HTTP/1.0 %d %s\r\nContent-Lenght: %d\r\n\r\n",
//               httpStatusCode, "OK", length);
//       printf("===== Responding with Header: =====\n%s", response);
//       int lenTaken = strlen(response);
//       std::cout << "Len taken: " << lenTaken << std::endl;

//       /* Read data */

//       // reaproveita o buffer response e concatena no fim
//       // TODO: checar para ver se os limites dos buffers estão ok
//       localFile.read(&response[lenTaken], BUFFER_SIZE - lenTaken);
//       int quantRead = localFile.gcount();

//       // Envia a resposta para o cliente
//       int quantSend = send(clientSockfd, response, lenTaken + quantRead,
//       0); std::cout << quantSend << std::endl; if (quantSend == -1) {
//         perror("send");
//         exit(6);
//       }

//       while (quantSend < length) {
//         localFile.read(response, BUFFER_SIZE);
//         quantRead = localFile.gcount();

//         // Envia a resposta para o cliente
//         int len = send(clientSockfd, response, quantRead, 0);
//         if (len == -1) {
//           perror("send");
//           exit(6);
//         }
//         std::cout << len << std::endl;
//         quantSend += len;
//       }

//       localFile.close();
//       break;
//   }

//   // fecha o socket
//   close(clientSockfd);
// }

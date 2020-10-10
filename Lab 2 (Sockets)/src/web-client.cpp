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

class HTTPReq {
public:
  char hostname[200];
  int port;
  char object[200];
  char method[20];

  void setURL(char* url){
      sscanf(url, "%*[^//]//%[^:]:%d/%s", hostname, &port, object);
  };

  void setMethod(char* m) {
    strcpy(method, m);
  }

  std::string buildRequest() {
    return std::string(method).append(" /").append(object).append(" HTTP/1.0");
  }

  // a partir do request, colocar tudo em bytes
  int encode() {
    return 1;
  }

  // a partir de bytes, preencher a classe
  void parse() {

  }

};

int main(int argc, char *argv[]) {

 for(int i = 1; i < argc; ++i ){
    HTTPReq request;

    // Cria a URL
    request.setURL(argv[i]);

    // transforma a url em bytes e manda
    int bytecode = request.encode();

    // cria o socket TCP IP
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // atividade de preenchimento da estrutura de endereço IP
    // e porta de conexão, precisa ser zerado o restante da estrutura
    // struct sockaddr_in {
    //  short            sin_family;   // e.g. AF_INET, AF_INET6
    //  unsigned short   sin_port;     // e.g. htons(3490)
    //  struct in_addr   sin_addr;     // see struct in_addr, below
    //  char             sin_zero[8];  // zero this if you want to
    // };
    
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

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(request.port);     // short, network byte order
    //std::cout << request.hostname;
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

    // trecho de código para leitura e envio de dados nessa conexao
    // buffer eh o buffer de dados a ser recebido no socket com 100 bytes
    bool isEnd = false;
    char buf[1050] = {0};
   
    std::stringstream ss;
    unsigned char header[2000] = {0};
    char data[2000]   = {0};
    char *token;

    // envia a msg
    request.setMethod("GET");
    std::string input = request.buildRequest();
    std::cout << "sendin: " + input << std::endl;
    if (send(sockfd, input.c_str(), input.size(), 0) == -1) {
      perror("send");
      return 4;
    }

    // zera o buffer
    memset(buf, '\0', sizeof(buf));

    // recebe no buffer uma certa quantidade de bytes ate 2000
    if (recv(sockfd, buf, 200, 0) == -1) {
      perror("recv");
      return 5;
    }

    ss << buf;

    std::string linee;
    int responseStatus = 0;

    sscanf(buf, "HTTP/1.0 %d%*s", &responseStatus);
    std::cout << "status: " << responseStatus << std::endl;

    switch (responseStatus)
    {
    case 200:
    {
      int conntentLenght = 0;

      //take tamanho
      sscanf(buf, "HTTP/1.0 %d%*[^Content-Lenght:]Content-Lenght: %d%*s", &responseStatus, &conntentLenght);
      std::cout << "conntentLenght: " << conntentLenght << std::endl;

      std::string st = ss.str();
      std::string rest = st.substr(st.find("\r\n\r\n") + 4, st.size());
      //std::string rest = st.substr(st.find("\n\n") + 2, st.size());
      int received = rest.size();

      std::cout << "\nRecebido: \n" << rest;
      std::cout << "\nsizerest: " << rest.size(); 

      while(received <= conntentLenght){
          // zera o buffer
          memset(buf, '\0', sizeof(buf));
          ss.str("");

          // recebe no buffer uma certa quantidade de bytes ate 2000
          if (recv(sockfd, buf, 200, 0) == -1) {
            perror("recv");
            return 5;
          }
          ss << buf;

          rest = ss.str();
          std::cout << "\nRecebido: \n" << rest << "\n ";
          std::cout << "\nsizerest: " << rest.size(); 
          received += rest.size();
          std::cout << "\nReceived until now: " << received; 
      }
      break;
    }
    default:
      printf("%s\n", buf);
      break;
    }
    
    // Keep printing tokens while one of the 
    // delimiters present in str[]. 
    /*while (token != NULL) 
    { 
        printf("opa%s\n", token); 
        token = strtok(NULL, "\n\n"); 
    } */


    /*while (!isEnd) {
      // zera o buffer
      memset(buf, '\0', sizeof(buf));

      // recebe no buffer uma certa quantidade de bytes ate 2000
      if (recv(sockfd, buf, 2000, 0) == -1) {
        perror("recv");
        return 5;
      }

      // coloca o conteudo do buffer na string
      // imprime o buffer na tela
      ss << buf << std::endl;
      std::cout << "echo: ";
      std::cout << buf << std::endl;



      // se a string tiver o valor close, sair do loop de eco
      if (ss.str() == "close\n")
        break;

      // zera a string ss
      ss.str("");
    }*/

    // fecha o socket
    close(sockfd);

  }
  return 0;
}

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

#define BUFFER_SIZE 1024


void print_header(char*, char*, char*, char*);
 
int main(int argc, char *argv[]) {
    // [x] TODO: Processar os argumentos da main ===================================================
    char *arg_host;
    char default_arg_host[10] = "localhost";
    char *arg_port;
    char default_arg_port[5] = "3000";
    char *arg_dir;
    char default_arg_dir[5] = "/tmp";
    
    // Se uso incorreto ou usuário pedindo ajuda mostrar mensagem de "usage"
    if (argc > 4 || 
       (argc > 1 && (strcmp(argv[1], "-help")  == 0 || 
                     strcmp(argv[1], "--help") == 0 ||
                     strcmp(argv[1], "-h")     == 0 ))) 
    {
        std::cerr << "Usage: web-server [host] [port] [dir]\n\n" << std::endl;
        return 1;
    }
    else {
        arg_host = (argc > 1 ? argv[1] : default_arg_host);
        arg_port = (argc > 2 ? argv[2] : default_arg_port);
        arg_dir  = (argc > 3 ? argv[3] : default_arg_dir);
    }

    // [x] TODO: Resolver o endereço do HOSTNAME (DNS) ============================================
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
    if ((status = getaddrinfo(arg_host, "80", &hints, &res)) != 0) {
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

    // Print Header in StdOut =====================================================================
    print_header(arg_host, ipstr, arg_port, arg_dir);

    // [ ] TODO: Aceita uma solicitação HTTP ======================================================
    // cria um socket para IPv4 e usando protocolo de transporte TCP
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Opções de configuração do SOCKETs
    // No sistema Unix um socket local TCP fica preso e indisponível 
    // por algum tempo após close, a não ser que configurado SO_REUSEADDR
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        return 1;
    }

    // struct sockaddr_in {
    //  short            sin_family;   // e.g. AF_INET, AF_INET6
    //  unsigned short   sin_port;     // e.g. htons(3490)
    //  struct in_addr   sin_addr;     // see struct in_addr, below
    //  char             sin_zero[8];  // zero this if you want to
    // };

    // Convert port string to integer
    std::stringstream stream_port(arg_port);
    int port = 0;
    stream_port >> port;

    // Connect
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);     // porta tem 16 bits, logo short, network byte order
    addr.sin_addr.s_addr = inet_addr(ipstr);
    memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

    // realizar o bind (registrar a porta para uso com o SO) para o socket
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        return 2;
    }

    // colocar o socket em modo de escuta, ouvindo a porta 
    if (listen(sockfd, 1) == -1) {
        perror("listen");
        return 3;
    }

    // aceitar a conexao TCP
    // verificar que sockfd e clientSockfd sao sockets diferentes
    // sockfd eh a "socket de boas vindas"
    // clientSockfd eh a "socket diretamente com o cliente"
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);

    
    // Receber e processar a requisição HTTP
    // COMMENTS: Fazer como se fosse via HTTP 1.0, um socket por objeto ===============================
    while (true) {
        int clientSockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrSize);

        if (clientSockfd == -1) {
            perror("accept");
            return 4;
        }

        // usa um vetor de caracteres para preencher o endereço IP do cliente
        ipstr[INET_ADDRSTRLEN] = {'\0'};

        // faz leitura e escrita dos dados da conexao
        // utiliza um buffer de BUFFER_SIZE bytes (char)
        bool isEnd = false;
        char buf[BUFFER_SIZE + 1] = {0};

        
        // zera a memoria do buffer
        memset(buf, '\0', sizeof(buf));

        // recebe ate 1024 bytes do cliente remoto
        if (recv(clientSockfd, buf, BUFFER_SIZE, 0) == -1) {
            perror("recv");
            return 5;
        }

        // [+/-] TODO: Parse de HTTP ====================================================================
        //     HTTP válido: 
        //         GET /[path]/[filename] HTTP/1.1
        //
        //     !!! Verificar se o protocolo é só isso mesmo !!!
        char filename[200];
        char complete_filename[300] = "\0";
        char http_version[5];
        if (sscanf(buf, "GET /%s HTTP/%s\n %*s", filename, http_version) == 2) {
            printf("Filename: %s\nVersion: %s\n", filename, http_version);
        }
        else {
            printf("ERROR\n");
        }

        // [ ] TODO: Impedir path traversal
        strcat(complete_filename, arg_dir);
        strcat(complete_filename, "/");
        strcat(complete_filename, filename);
        printf("Complete filename: %s\n", complete_filename);

        // [x] TODO: Procura o arquivo solicitado =====================================================
        FILE *local_file;
        local_file = fopen(complete_filename, "r");

        if (local_file == NULL) {
            // Tratar erros (404)
            printf("ARQUIVO NÃO ENCONTRADO: %s", complete_filename);
        }

        // Código retirado da internet - Fonte: https://stackoverflow.com/questions/2029103/correct-way-to-read-a-text-file-into-a-buffer-in-c
        char source[BUFFER_SIZE + 1];
        
        // Criar uma resposta
        // [ ] TODO: Quebrar o arquivo em chunks
        
        /* Seek to the beginning of the file */
        fseek(local_file, 0, SEEK_END);
        long lSize = ftell(local_file);
        rewind(local_file);

        /* Read data */
        char file_contents[200];
        fread(file_contents, 1, lSize, local_file);
        fclose(local_file);
       
        char response_str[BUFFER_SIZE + 1];
        sprintf(response_str, "HTTP/1.0 200 OK\nContent-Lenght: %d\n\n\n", http_version, lSize);
        printf("%s\n\n", response_str);


        /* Concatenate file contents and Responde headers */
        strcat(response_str, file_contents);

        // [ ] TODO: Retorna o conteúdo (se existir) ==================================================
        
        // [ ] TODO: Caso contrário responder com o código apropriado (404) ===========================

        // Imprime o valor recebido no servidor antes de reenviar
        // para o cliente de volta
        // ss << buf << std::endl;
        // std::cout << buf << std::endl;

        // envia de volta o buffer recebido como um echo
        // if (send(clientSockfd, buf, BUFFER_SIZE, 0) == -1) {
        // perror("send");
        // return 6;
        // }

        //printf("Mensagem: \n%s\n\n", response_str);

        // Envia o conteúdo do arquivo como resposta
        if (send(clientSockfd, response_str, strlen(response_str), 0) == -1) {
            perror("send");
            return 6;
        }

        // fecha o socket
        close(clientSockfd);
    }

    // TAREFAS ESPERADAS:
    //  [x] a) Converter o nome do host do servidor em endereço IP, abrir socket para escuta neste  endereço IP e no número de porta especificado
    //  [x] b) Por meio do socket "listen" aceitar solicitações de conexão dos clientes, e estabelecer conexões com os clientes.
    //  [ ] c) Fazer  uso  de  programação  de  redes  que  lide  com  conexões simultâneas (por exemplo, por meio de multiprocess e multithreads). Ou seja, o servidor web deve poder receber solicitações de vários clientes ao mesmo tempo.
    return 0;
}

void print_header(char *host, char *translated_host, char *port, char *dir) {
    printf("===== STARTING AWESOME SERVER =====\n\n");
    if(strcmp(host, translated_host) == 0) {
        printf("Host: %s\nPort: %s\nDirectory: %s\n\n", host, port, dir);
    }
    else {
        printf("Host: %s (%s)\nPort: %s\nDirectory: %s\n\n", host, translated_host, port, dir);
    }
    printf("===================================\n\n");
}

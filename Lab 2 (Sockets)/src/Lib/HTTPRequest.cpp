#include "./HTTPRequest.hpp"


void HTTPRequest::setURL(char* url){
      memset(object, '\0', sizeof(object));
      sscanf(url, "%*[^//]//%[^:]:%d/%s", hostname, &port, object);

      if(object[0] == '\0' || object[0] == '/')
         strcpy(object, "index.html");

      //std::cout << "hostname: " << hostname << std::endl;
      //std::cout << "port: "     << port     << std::endl;
      //std::cout << "object: "   << object   << std::endl;
  };

void HTTPRequest::setMethod(const char* m) {
    strcpy(method, m);
  }

 std::string HTTPRequest::buildRequest() {
    return std::string(method).append(" /").append(object).append(" HTTP/1.0");
  }

int HTTPRequest::getResponseStatus(char* buf){
    int responseStatus;
    sscanf(buf, "HTTP/1.0 %d%*s", &responseStatus);
    return responseStatus;
  }

int HTTPRequest::getContentLenght(char* buf){
    int aux, contentLenght;
    sscanf(buf, "HTTP/1.0 %d%*[^Content-Lenght:]Content-Lenght: %d%*s", &aux, &contentLenght);
    std::cout << "Content-Lenght: " << contentLenght << std::endl;
    return contentLenght;
  }

const char* HTTPRequest::getObject(){
    if(object[0] == '\0' || object[0] == '/')
         return "index.html";

    return object;
  }

void HTTPRequest::handle(char* buf2, char* buf, int &received){
    int position = 0;
    int notfound = 1;
    for(int i = 0; i < BUFFER_SIZE - 1 && notfound; ++i) {
      if (buf[i] == '\r'){
        ++i;
        if(i < BUFFER_SIZE - 1 && buf[i] == '\n'){
          ++i;
          if(i < BUFFER_SIZE - 1 && buf[i] == '\r'){
            ++i;
            if(i < BUFFER_SIZE - 1 && buf[i] == '\n'){
              notfound = 0;
              position = i + 1;
            }
          }
        }
      }
    }
    //std::cout << "start of data: " << position << std::endl;
    memset(buf2, '\0', sizeof(buf2));
    for (int i = position; i < received; ++i){
      buf2[i-position] = buf[i];
    }

    received -= position;

  }

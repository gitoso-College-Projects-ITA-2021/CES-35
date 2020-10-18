#include "./HTTPResponse.hpp"

#include <sstream>

HTTP::StatusCode HTTP::Response::getStatusCode() { return this->statusCode; };

void HTTP::Response::setStatusCode(StatusCode status) {
  this->statusCode = status;
}

void HTTP::Response::decode(std::string& payload) {
  // formato da resposta HTTP
  const char* format = "HTTP/1.0 %d %s\r\n\r\n";

  int size = 0;
  switch (this->statusCode) {
    case HTTP::OK:
      size = std::snprintf(nullptr, 0, format, this->statusCode, "OK");
      payload.resize(size + 1);
      std::snprintf(&payload[0], payload.size(), format, this->statusCode,
                    "OK");
      break;

    case HTTP::BadRequest:
      size = std::snprintf(nullptr, 0, format, this->statusCode, "Bad Request");
      payload.resize(size + 1);
      std::snprintf(&payload[0], payload.size(), format, this->statusCode,
                    "OK");
      break;

    case HTTP::NotFound:
      size = std::snprintf(nullptr, 0, format, this->statusCode, "Not Found");
      payload.resize(size + 1);
      std::snprintf(&payload[0], payload.size(), format, this->statusCode,
                    "OK");
      break;
  }
}

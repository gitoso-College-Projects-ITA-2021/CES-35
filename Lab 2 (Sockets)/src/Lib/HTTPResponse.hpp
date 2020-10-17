#include <cstdint>
#include <vector>

enum StatusCode { OK = 200, BadRequest = 400, NotFound = 404 };

class HTTPResponse {
 public:
  void setStatusCode(int);
  std::vector<uint8_t>& decode(void);

 private:
  int statusCode;
  int contentLength;
};

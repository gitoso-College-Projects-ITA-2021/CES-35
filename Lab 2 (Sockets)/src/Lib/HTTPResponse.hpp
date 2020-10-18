#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace HTTP {
enum StatusCode { OK = 200, BadRequest = 400, NotFound = 404 };

class Response {
 public:
  StatusCode getStatusCode(void);
  void setStatusCode(StatusCode);
  void decode(std::string&);

 private:
  StatusCode statusCode;
  int contentLength;
};
}  // namespace HTTP

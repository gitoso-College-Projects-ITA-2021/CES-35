#include <cstdint>
#include <string>
#include <vector>

namespace HTTP {
class Request {
 public:
  static void parse(std::vector<uint8_t>& bytecode);
  void setURL(std::string url);
  void setMethod(std::string method);
  std::vector<uint8_t>& encode(void);
};
}  // namespace HTTP

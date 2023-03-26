
#include <ctime>
#include <memory>
#include <queue>
#include "logfile.h"
#include "noncopyable.h"

using namespace tiny_muduo;

class LogManager : public tiny_muduo::NonCopyAble {
  public:
  
  LogManager();
  void RemoveOldLog();
  void Add(LogInfo info);
  void Setinterval(std::time_t);
 private:
  
  std::time_t interval_;
  std::unique_ptr<std::queue<LogInfo>> histroy_;
};
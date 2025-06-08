#pragma once

#include <rime/common.h>
#include <rime/context.h>
#include <rime/engine.h>
#include <rime/processor.h>

namespace rime {

struct Log;

class KeyLogger : public Processor {
 public:
  explicit KeyLogger(const Ticket& ticket);
  virtual ~KeyLogger();

  void StartLogging();
  void EndLogging();

  ProcessResult ProcessKeyEvent(const KeyEvent& key_event) override;

 private:
  void OnUpdate(Context* ctx) {}
  void OnOptionUpdate(Context* ctx, const string& option);

  connection update_connection_;
  connection option_update_connection_;

  vector<Log> logs_;
  string log_file_;
  bool is_logging_;
};

}  // namespace rime

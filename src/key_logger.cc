#include "key_logger.h"

#include <fstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <rime/key_event.h>
#include <yaml-cpp/yaml.h>

using namespace boost::posix_time;

namespace rime {

struct Log {
  ptime timestamp;
  KeyEvent key_event;
};

YAML::Emitter& operator<< (YAML::Emitter& out, const Log& log) {
  out << YAML::BeginMap
    << YAML::Key << "time" << YAML::Value << to_iso_string(log.timestamp)
    << YAML::Key << "key" << YAML::Value << log.key_event.repr()
    << YAML::EndMap;
  return out;
}

KeyLogger::KeyLogger(const Ticket& ticket)
  : Processor(ticket) {
  Context* context = engine_->context();
  update_connection_ = context->update_notifier()
    .connect([this](Context* ctx) { OnUpdate(ctx); });
  option_update_connection_ = context->option_update_notifier()
    .connect([this](Context* ctx, const string& option) {
      OnOptionUpdate(ctx, option);
    });
}

KeyLogger::~KeyLogger() {
  update_connection_.disconnect();
  if (is_logging_) {
    EndLogging();
  }
}

void KeyLogger::OnOptionUpdate(Context* ctx, const string& option) {
  if (option == "_key_logger") {
    if (ctx->get_option(option)) {
      StartLogging();
    } else {
      EndLogging();
    }
  }
}

void KeyLogger::StartLogging() {
  if (is_logging_)
    return;
  ptime now = microsec_clock::local_time();
  log_file_ = to_iso_string(now) + ".klog";
  is_logging_ = true;
}

void KeyLogger::EndLogging() {
  if (is_logging_) {
    if (!logs_.empty()) {
      // output to file.
      YAML::Emitter out;
      out << logs_;
      std::ofstream yaml_file(log_file_);
      yaml_file << out.c_str();
    }
    logs_.clear();
    log_file_ = string();
    is_logging_ = false;
  }
}

ProcessResult KeyLogger::ProcessKeyEvent(const KeyEvent& key_event) {
  if (engine_->context()->get_option("_key_logger")) {
    ptime now = microsec_clock::local_time();
    // LOG(INFO) << to_iso_string(now) << " key: " << key_event;
    logs_.push_back(Log {now, key_event});
  }
  return kNoop;
}

}  // namespace rime

#include "key_logger.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <rime/key_event.h>

namespace rime {

using namespace boost::posix_time;

ProcessResult KeyLogger::ProcessKeyEvent(const KeyEvent& key_event) {
  if (engine_->context()->get_option("key_logger")) {
    ptime now = microsec_clock::local_time();
    LOG(INFO) << to_iso_string(now) << " key: " << key_event;
  }
  return kNoop;
}

}  // namespace rime

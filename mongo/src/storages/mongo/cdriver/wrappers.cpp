#include <storages/mongo/cdriver/wrappers.hpp>

#include <unistd.h>

#include <chrono>
#include <mutex>
#include <utility>

#include <mongoc/mongoc.h>

#include <userver/crypto/openssl.hpp>
#include <userver/engine/task/task.hpp>
#include <userver/logging/log.hpp>
#include <userver/utils/assert.hpp>
#include <userver/utils/userver_info.hpp>

#include <storages/mongo/cdriver/logger.hpp>

USERVER_NAMESPACE_BEGIN

namespace storages::mongo::impl::cdriver {

GlobalInitializer::GlobalInitializer() {
  crypto::Openssl::Init();
  mongoc_log_set_handler(&LogMongocMessage, nullptr);
  mongoc_init();
  mongoc_handshake_data_append("userver", utils::GetUserverVcsRevision(),
                               nullptr);
}

GlobalInitializer::~GlobalInitializer() { mongoc_cleanup(); }

void GlobalInitializer::LogInitWarningsOnce() {
  static std::once_flag once_flag;
  std::call_once(once_flag, [] {
#if !MONGOC_CHECK_VERSION(1, 26, 0)
    LOG_WARNING() << "Cannot use coro-friendly usleep in mongo driver, "
                     "link against newer mongo-c-driver to fix";
#endif
  });
}

ReadPrefsPtr::ReadPrefsPtr(mongoc_read_mode_t read_mode)
    : read_prefs_(mongoc_read_prefs_new(read_mode)) {}

ReadPrefsPtr::~ReadPrefsPtr() { Reset(); }

ReadPrefsPtr::ReadPrefsPtr(const ReadPrefsPtr& other) { *this = other; }

ReadPrefsPtr::ReadPrefsPtr(ReadPrefsPtr&& other) noexcept {
  *this = std::move(other);
}

ReadPrefsPtr& ReadPrefsPtr::operator=(const ReadPrefsPtr& rhs) {
  if (this == &rhs) return *this;

  Reset();
  read_prefs_ = mongoc_read_prefs_copy(rhs.read_prefs_);
  return *this;
}

ReadPrefsPtr& ReadPrefsPtr::operator=(ReadPrefsPtr&& rhs) noexcept {
  Reset();
  read_prefs_ = std::exchange(rhs.read_prefs_, nullptr);
  return *this;
}

ReadPrefsPtr::operator bool() const { return !!Get(); }
const mongoc_read_prefs_t* ReadPrefsPtr::Get() const { return read_prefs_; }
mongoc_read_prefs_t* ReadPrefsPtr::Get() { return read_prefs_; }

void ReadPrefsPtr::Reset() noexcept {
  if (read_prefs_) {
    mongoc_read_prefs_destroy(std::exchange(read_prefs_, nullptr));
  }
}

}  // namespace storages::mongo::impl::cdriver

USERVER_NAMESPACE_END

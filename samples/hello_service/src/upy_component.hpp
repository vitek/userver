#pragma once

#include <pytypedefs.h>

#include <userver/components/component_base.hpp>
#include <userver/engine/mutex.hpp>
#include <userver/engine/task/single_threaded_task_processors_pool.hpp>

namespace upython {

struct Interpreter {
  PyThreadState *tstate;
  PyObject *func;
};

class Component final : public userver::components::LoggableComponentBase {
 public:
  static constexpr std::string_view kName{"upython"};

  Component(const userver::components::ComponentConfig& config,
            const userver::components::ComponentContext& context);


  std::string RunScript();

 private:
  userver::engine::Mutex mutex_;
  userver::engine::SingleThreadedTaskProcessorsPool &pool_;
  std::vector<std::unique_ptr<Interpreter>> interpreters_;
  std::atomic_size_t idx_;
};

}  // namespace upython

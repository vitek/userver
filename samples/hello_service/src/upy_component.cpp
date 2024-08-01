#include "upy_component.hpp"

#include <stdexcept>

#include <userver/components/component_context.hpp>
#include <userver/components/single_threaded_task_processors.hpp>
#include <userver/logging/log.hpp>
#include <userver/utils/async.hpp>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

namespace upython {

namespace {

std::unique_ptr<Interpreter> PythonRuntimeInit(size_t idx) {
  PyInterpreterConfig config = {
    .check_multi_interp_extensions = 1,
    .gil = PyInterpreterConfig_OWN_GIL,
  };
  PyThreadState *tstate = nullptr;
  PyStatus status = Py_NewInterpreterFromConfig(&tstate, &config);
  if (PyStatus_Exception(status)) {
    return nullptr;
  }

  PyObject* pName = nullptr;
  PyObject* pModule = nullptr;

  pName = PyUnicode_FromString("upy");
  pModule = PyImport_Import(pName);

  Py_DECREF(pName);

  if (!pModule) {
    PyErr_Print();
    throw std::runtime_error("oops");
  }

  PyObject* pFunc = PyObject_GetAttrString(pModule, "init");
  Py_XDECREF(pModule);

  if (!pFunc) {
    PyErr_Print();
    Py_DECREF(pModule);
    throw std::runtime_error("oops");
  }

  PyObject* pResult = PyObject_CallFunction(pFunc, "n", idx);

  Py_XDECREF(pFunc);

  if (!pResult) {
    PyErr_Print();
    throw std::runtime_error("oops");
  }

  return std::make_unique<Interpreter>(Interpreter{
    .tstate = tstate,
    .func = pResult,
    });
}

}  // namespace

Component::Component(
    [[maybe_unused]] const userver::components::ComponentConfig& config,
    [[maybe_unused]] const userver::components::ComponentContext& context)
    : userver::components::LoggableComponentBase{config, context},
      pool_(context
                .FindComponent<
                    userver::components::SingleThreadedTaskProcessors>()
                .GetPool()) {
  Py_Initialize();

  std::vector<userver::engine::TaskWithResult<std::unique_ptr<Interpreter>>> features;

  for (size_t idx = 0; idx < pool_.GetSize(); idx++) {
    auto& task_processor = pool_.At(idx);
    auto feature = userver::utils::Async(task_processor, "upy-init", [&, idx] {
      return PythonRuntimeInit(idx);
    });
    features.push_back(std::move(feature));
  }

  for (auto &feature: features) {
    interpreters_.push_back(feature.Get());
  }
}

std::string Component::RunScript() {
  size_t idx = idx_++ % pool_.GetSize();
  auto& task_processor = pool_.At(idx);

  auto feature = userver::utils::Async(task_processor, "upy-script", [&]() {
    auto *interpreter = interpreters_[idx].get();

    PyObject* pyResult = PyObject_CallNoArgs(interpreter->func);

    if (!pyResult) {
      PyErr_Print();
      throw std::runtime_error("oops");
    }

    const char* retval = PyUnicode_AsUTF8(pyResult);
    std::string result = retval;
    Py_DECREF(pyResult);
    return result;
  });

  return feature.Get();
}

}  // namespace upython

#pragma once

#include "string.hpp"

namespace ns {

static constexpr USERVER_NAMESPACE::utils::TrivialSet
    kns__String_PropertiesNames = [](auto selector) {
      return selector().template Type<std::string>().Case("foo");
    };

template <typename Value>
ns::String Parse(Value value,
                 USERVER_NAMESPACE::formats::parse::To<ns::String>) {
  value.CheckNotMissing();
  value.CheckObjectOrNull();

  ns::String res;

  res.foo = value["foo"]
                .template As<std::optional<
                    USERVER_NAMESPACE::chaotic::Primitive<std::string>>>();

  USERVER_NAMESPACE::chaotic::ValidateNoAdditionalProperties(
      value, kns__String_PropertiesNames);

  return res;
}

}  // namespace ns

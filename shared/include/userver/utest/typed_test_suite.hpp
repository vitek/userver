#pragma once

/// @file userver/utest/typed_test_suite.hpp
/// @brief Provides TYPED_TEST_SUITE* macros for gtest versions that
/// provide only TYPED_TEST_CASE along with the gtest/gtest.h include.

#include <gtest/gtest.h>

#ifndef TYPED_TEST_SUITE
#define TYPED_TEST_SUITE TYPED_TEST_CASE
#endif

#ifndef TYPED_TEST_SUITE_P
#define TYPED_TEST_SUITE_P TYPED_TEST_CASE_P
#endif

#ifndef REGISTER_TYPED_TEST_SUITE_P
#define REGISTER_TYPED_TEST_SUITE_P REGISTER_TYPED_TEST_CASE_P
#endif

#ifndef INSTANTIATE_TYPED_TEST_SUITE_P
#define INSTANTIATE_TYPED_TEST_SUITE_P INSTANTIATE_TYPED_TEST_CASE_P
#endif

#ifndef INSTANTIATE_TEST_SUITE_P
#define INSTANTIATE_TEST_SUITE_P INSTANTIATE_TEST_CASE_P
#endif

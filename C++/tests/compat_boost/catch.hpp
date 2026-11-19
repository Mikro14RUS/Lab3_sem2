// Слой совместимости: позволяет существующим тестам Catch2 работать под Boost.Test.
//
// BOOST_TEST_MODULE определяется здесь, потому что каждый тестовый файл
// собирается в отдельный исполняемый файл. Отображение сохраняет семантику:
// REQUIRE останавливает текущий тест, CHECK фиксирует ошибку и продолжает.
#ifndef LAB3_COMPAT_BOOST_HPP
#define LAB3_COMPAT_BOOST_HPP

#ifndef BOOST_TEST_MODULE
#define BOOST_TEST_MODULE Lab3BoostCompat
#endif

#include <boost/test/unit_test.hpp>

#define LAB3_BOOST_CAT_(a, b) a##b
#define LAB3_BOOST_CAT(a, b) LAB3_BOOST_CAT_(a, b)
#define TEST_CASE(name) BOOST_AUTO_TEST_CASE(LAB3_BOOST_CAT(case_, __LINE__))

#define REQUIRE(...) BOOST_REQUIRE((__VA_ARGS__))
#define REQUIRE_FALSE(...) BOOST_REQUIRE(!(__VA_ARGS__))
#define CHECK(...) BOOST_CHECK((__VA_ARGS__))
#define CHECK_FALSE(...) BOOST_CHECK(!(__VA_ARGS__))

// Boost.Test не требует контекста CAPTURE из Catch2 в этих тестах.
#define CAPTURE(...) ((void)0)

#endif  // LAB3_COMPAT_BOOST_HPP

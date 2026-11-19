// Слой совместимости: позволяет существующим тестам Catch2 работать под GoogleTest.
//
// В проекте один основной набор тестовых сценариев. Этот заголовок
// отображает небольшой набор макросов Catch2 (TEST_CASE, REQUIRE, CHECK)
// в проверки GoogleTest. Каждый тестовый файл собирается в отдельный
// исполняемый файл, поэтому имена на основе __LINE__ уникальны внутри него.
#ifndef LAB3_COMPAT_GTEST_HPP
#define LAB3_COMPAT_GTEST_HPP

#include <gtest/gtest.h>

#define LAB3_GTEST_CAT_(a, b) a##b
#define LAB3_GTEST_CAT(a, b) LAB3_GTEST_CAT_(a, b)
#define TEST_CASE(name) TEST(Lab3Compat, LAB3_GTEST_CAT(case_, __LINE__))

#define REQUIRE(...) ASSERT_TRUE((__VA_ARGS__))
#define REQUIRE_FALSE(...) ASSERT_FALSE((__VA_ARGS__))
#define CHECK(...) EXPECT_TRUE((__VA_ARGS__))
#define CHECK_FALSE(...) EXPECT_FALSE((__VA_ARGS__))

// CAPTURE в Catch2 добавляет только диагностический контекст; GoogleTest
// выводит выражение и место ошибки, чего достаточно для этих тестов.
#define CAPTURE(...) ((void)0)

#endif  // LAB3_COMPAT_GTEST_HPP

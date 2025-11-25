#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace testing {

class Test {
public:
    virtual ~Test() = default;
    virtual void SetUp() {}
    virtual void TearDown() {}
    virtual void TestBody() {}
};

namespace internal {

struct TestInfo {
    const char* suite_name;
    const char* test_name;
    std::function<Test*()> factory;
};

inline std::vector<TestInfo>& registry() {
    static std::vector<TestInfo> tests;
    return tests;
}

inline void register_test(const char* suite, const char* name, std::function<Test*()> factory) {
    registry().push_back(TestInfo{suite, name, std::move(factory)});
}

inline int& failure_count() {
    static int failures = 0;
    return failures;
}

template <typename T> inline std::string format_value(const T& value) {
    if constexpr (std::is_same_v<T, bool>) {
        return value ? "true" : "false";
    } else if constexpr (std::is_same_v<T, const char*> || std::is_same_v<T, char*>) {
        return value ? std::string(value) : std::string("(null)");
    } else if constexpr (std::is_convertible_v<T, std::string_view>) {
        return std::string(std::string_view(value));
    } else if constexpr (std::is_enum_v<T>) {
        using underlying = std::underlying_type_t<T>;
        return format_value(static_cast<underlying>(value));
    } else if constexpr (std::is_integral_v<T>) {
        std::ostringstream oss;
        if constexpr (std::is_unsigned_v<T>) {
            oss << static_cast<std::uintmax_t>(value);
        } else {
            oss << static_cast<std::intmax_t>(value);
        }
        return oss.str();
    } else if constexpr (std::is_pointer_v<T>) {
        std::ostringstream oss;
        oss << static_cast<const void*>(value);
        return oss.str();
    } else if constexpr (requires(std::ostringstream& oss, const T& v) { oss << v; }) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    } else {
        return "<unprintable>";
    }
}

inline bool report_failure(bool failed,
                           const char* expression,
                           const char* file,
                           int line,
                           bool fatal,
                           std::string_view details = {}) {
    if (!failed) {
        return false;
    }

    ++failure_count();
    std::cerr << file << ":" << line << ": " << (fatal ? "FATAL" : "FAIL") << " [" << expression
              << "]";
    if (!details.empty()) {
        std::cerr << " " << details;
    }
    std::cerr << std::endl;
    return fatal;
}

template <typename A, typename B>
inline bool compare_equal(
    const A& lhs, const B& rhs, const char* expression, const char* file, int line, bool fatal) {
    if constexpr (std::is_arithmetic_v<A> && std::is_arithmetic_v<B>) {
        using common_t = std::common_type_t<A, B>;
        if (static_cast<common_t>(lhs) == static_cast<common_t>(rhs)) {
            return false;
        }
    } else {
        if (lhs == rhs) {
            return false;
        }
    }
    std::ostringstream oss;
    oss << "lhs=" << format_value(lhs) << " rhs=" << format_value(rhs);
    return report_failure(true, expression, file, line, fatal, oss.str());
}

template <typename A, typename B>
inline bool compare_not_equal(
    const A& lhs, const B& rhs, const char* expression, const char* file, int line, bool fatal) {
    if constexpr (std::is_arithmetic_v<A> && std::is_arithmetic_v<B>) {
        using common_t = std::common_type_t<A, B>;
        if (static_cast<common_t>(lhs) != static_cast<common_t>(rhs)) {
            return false;
        }
    } else {
        if (lhs != rhs) {
            return false;
        }
    }
    std::ostringstream oss;
    oss << "lhs=" << format_value(lhs) << " rhs=" << format_value(rhs);
    return report_failure(true, expression, file, line, fatal, oss.str());
}

template <typename A, typename B>
inline bool compare_less(
    const A& lhs, const B& rhs, const char* expression, const char* file, int line, bool fatal) {
    if constexpr (std::is_arithmetic_v<A> && std::is_arithmetic_v<B>) {
        using common_t = std::common_type_t<A, B>;
        if (static_cast<common_t>(lhs) < static_cast<common_t>(rhs)) {
            return false;
        }
    } else {
        if (lhs < rhs) {
            return false;
        }
    }
    std::ostringstream oss;
    oss << "lhs=" << format_value(lhs) << " rhs=" << format_value(rhs);
    return report_failure(true, expression, file, line, fatal, oss.str());
}

template <typename A, typename B>
inline bool compare_less_equal(
    const A& lhs, const B& rhs, const char* expression, const char* file, int line, bool fatal) {
    if constexpr (std::is_arithmetic_v<A> && std::is_arithmetic_v<B>) {
        using common_t = std::common_type_t<A, B>;
        if (static_cast<common_t>(lhs) <= static_cast<common_t>(rhs)) {
            return false;
        }
    } else {
        if (lhs <= rhs) {
            return false;
        }
    }
    std::ostringstream oss;
    oss << "lhs=" << format_value(lhs) << " rhs=" << format_value(rhs);
    return report_failure(true, expression, file, line, fatal, oss.str());
}

template <typename A, typename B>
inline bool compare_greater(
    const A& lhs, const B& rhs, const char* expression, const char* file, int line, bool fatal) {
    if constexpr (std::is_arithmetic_v<A> && std::is_arithmetic_v<B>) {
        using common_t = std::common_type_t<A, B>;
        if (static_cast<common_t>(lhs) > static_cast<common_t>(rhs)) {
            return false;
        }
    } else {
        if (lhs > rhs) {
            return false;
        }
    }
    std::ostringstream oss;
    oss << "lhs=" << format_value(lhs) << " rhs=" << format_value(rhs);
    return report_failure(true, expression, file, line, fatal, oss.str());
}

template <typename A, typename B>
inline bool compare_greater_equal(
    const A& lhs, const B& rhs, const char* expression, const char* file, int line, bool fatal) {
    if constexpr (std::is_arithmetic_v<A> && std::is_arithmetic_v<B>) {
        using common_t = std::common_type_t<A, B>;
        if (static_cast<common_t>(lhs) >= static_cast<common_t>(rhs)) {
            return false;
        }
    } else {
        if (lhs >= rhs) {
            return false;
        }
    }
    std::ostringstream oss;
    oss << "lhs=" << format_value(lhs) << " rhs=" << format_value(rhs);
    return report_failure(true, expression, file, line, fatal, oss.str());
}

inline bool
check_condition(bool condition, const char* expression, const char* file, int line, bool fatal) {
    return report_failure(!condition, expression, file, line, fatal);
}

template <typename TestType> class TestRegistrar {
public:
    TestRegistrar(const char* suite, const char* name) {
        register_test(suite, name, []() -> Test* { return new TestType(); });
    }
};

inline void announce_test(const char* suite, const char* name) {
    std::cout << "[ RUN      ] " << suite << "." << name << std::endl;
}

inline void report_test_result(const char* suite, const char* name, bool passed) {
    std::cout << (passed ? "[       OK ] " : "[  FAILED  ] ") << suite << "." << name << std::endl;
}

} // namespace internal

inline void InitGoogleTest(int* argc, char** argv) {
    (void)argc;
    (void)argv;
}

inline int RUN_ALL_TESTS() {
    auto& tests = internal::registry();
    int failed_before = internal::failure_count();
    std::cout << "[==========] Running " << tests.size() << " tests" << std::endl;

    for (const auto& info : tests) {
        int failures_before_test = internal::failure_count();
        internal::announce_test(info.suite_name, info.test_name);
        std::unique_ptr<Test> instance(info.factory());
        try {
            instance->SetUp();
            instance->TestBody();
            instance->TearDown();
        } catch (const std::exception& ex) {
            internal::report_failure(
                true, "Unhandled exception", __FILE__, __LINE__, false, ex.what());
        } catch (...) {
            internal::report_failure(
                true, "Unhandled exception", __FILE__, __LINE__, false, "unknown");
        }
        bool passed = internal::failure_count() == failures_before_test;
        internal::report_test_result(info.suite_name, info.test_name, passed);
    }

    int total_failures = internal::failure_count() - failed_before;
    if (total_failures == 0) {
        std::cout << "[  PASSED  ] All tests passed" << std::endl;
    } else {
        std::cout << "[  FAILED  ] " << total_failures << " test(s)" << std::endl;
    }
    return total_failures == 0 ? 0 : 1;
}

} // namespace testing

#define TEST(test_suite_name, test_name)                                                           \
    class test_suite_name##_##test_name##_Test : public ::testing::Test {                          \
    public:                                                                                        \
        void TestBody() override;                                                                  \
    };                                                                                             \
    static ::testing::internal::TestRegistrar<test_suite_name##_##test_name##_Test>                \
        test_suite_name##_##test_name##_registrar(#test_suite_name, #test_name);                   \
    void test_suite_name##_##test_name##_Test::TestBody()

#define TEST_F(test_fixture, test_name)                                                            \
    class test_fixture##_##test_name##_Test : public test_fixture {                                \
    public:                                                                                        \
        void TestBody() override;                                                                  \
    };                                                                                             \
    static ::testing::internal::TestRegistrar<test_fixture##_##test_name##_Test>                   \
        test_fixture##_##test_name##_registrar(#test_fixture, #test_name);                         \
    void test_fixture##_##test_name##_Test::TestBody()

#define EXPECT_TRUE(condition)                                                                     \
    do {                                                                                           \
        auto&& gtest_cond = (condition);                                                           \
        ::testing::internal::check_condition(                                                      \
            static_cast<bool>(gtest_cond), #condition, __FILE__, __LINE__, false);                 \
    } while (0)

#define EXPECT_FALSE(condition)                                                                    \
    do {                                                                                           \
        auto&& gtest_cond = (condition);                                                           \
        ::testing::internal::check_condition(                                                      \
            !static_cast<bool>(gtest_cond), #condition, __FILE__, __LINE__, false);                \
    } while (0)

#define ASSERT_TRUE(condition)                                                                     \
    do {                                                                                           \
        auto&& gtest_cond = (condition);                                                           \
        if (::testing::internal::check_condition(                                                  \
                static_cast<bool>(gtest_cond), #condition, __FILE__, __LINE__, true)) {            \
            return;                                                                                \
        }                                                                                          \
    } while (0)

#define ASSERT_FALSE(condition)                                                                    \
    do {                                                                                           \
        auto&& gtest_cond = (condition);                                                           \
        if (::testing::internal::check_condition(                                                  \
                !static_cast<bool>(gtest_cond), #condition, __FILE__, __LINE__, true)) {           \
            return;                                                                                \
        }                                                                                          \
    } while (0)

#define EXPECT_EQ(val1, val2)                                                                      \
    do {                                                                                           \
        const auto& gtest_lhs = (val1);                                                            \
        const auto& gtest_rhs = (val2);                                                            \
        ::testing::internal::compare_equal(                                                        \
            gtest_lhs, gtest_rhs, #val1 " == " #val2, __FILE__, __LINE__, false);                  \
    } while (0)

#define ASSERT_EQ(val1, val2)                                                                      \
    do {                                                                                           \
        const auto& gtest_lhs = (val1);                                                            \
        const auto& gtest_rhs = (val2);                                                            \
        if (::testing::internal::compare_equal(                                                    \
                gtest_lhs, gtest_rhs, #val1 " == " #val2, __FILE__, __LINE__, true)) {             \
            return;                                                                                \
        }                                                                                          \
    } while (0)

#define EXPECT_NE(val1, val2)                                                                      \
    do {                                                                                           \
        const auto& gtest_lhs = (val1);                                                            \
        const auto& gtest_rhs = (val2);                                                            \
        ::testing::internal::compare_not_equal(                                                    \
            gtest_lhs, gtest_rhs, #val1 " != " #val2, __FILE__, __LINE__, false);                  \
    } while (0)

#define ASSERT_NE(val1, val2)                                                                      \
    do {                                                                                           \
        const auto& gtest_lhs = (val1);                                                            \
        const auto& gtest_rhs = (val2);                                                            \
        if (::testing::internal::compare_not_equal(                                                \
                gtest_lhs, gtest_rhs, #val1 " != " #val2, __FILE__, __LINE__, true)) {             \
            return;                                                                                \
        }                                                                                          \
    } while (0)

#define EXPECT_LT(val1, val2)                                                                      \
    do {                                                                                           \
        const auto& gtest_lhs = (val1);                                                            \
        const auto& gtest_rhs = (val2);                                                            \
        ::testing::internal::compare_less(                                                         \
            gtest_lhs, gtest_rhs, #val1 " < " #val2, __FILE__, __LINE__, false);                   \
    } while (0)

#define ASSERT_LT(val1, val2)                                                                      \
    do {                                                                                           \
        const auto& gtest_lhs = (val1);                                                            \
        const auto& gtest_rhs = (val2);                                                            \
        if (::testing::internal::compare_less(                                                     \
                gtest_lhs, gtest_rhs, #val1 " < " #val2, __FILE__, __LINE__, true)) {              \
            return;                                                                                \
        }                                                                                          \
    } while (0)

#define EXPECT_LE(val1, val2)                                                                      \
    do {                                                                                           \
        const auto& gtest_lhs = (val1);                                                            \
        const auto& gtest_rhs = (val2);                                                            \
        ::testing::internal::compare_less_equal(                                                   \
            gtest_lhs, gtest_rhs, #val1 " <= " #val2, __FILE__, __LINE__, false);                  \
    } while (0)

#define ASSERT_LE(val1, val2)                                                                      \
    do {                                                                                           \
        const auto& gtest_lhs = (val1);                                                            \
        const auto& gtest_rhs = (val2);                                                            \
        if (::testing::internal::compare_less_equal(                                               \
                gtest_lhs, gtest_rhs, #val1 " <= " #val2, __FILE__, __LINE__, true)) {             \
            return;                                                                                \
        }                                                                                          \
    } while (0)

#define EXPECT_GT(val1, val2)                                                                      \
    do {                                                                                           \
        const auto& gtest_lhs = (val1);                                                            \
        const auto& gtest_rhs = (val2);                                                            \
        ::testing::internal::compare_greater(                                                      \
            gtest_lhs, gtest_rhs, #val1 " > " #val2, __FILE__, __LINE__, false);                   \
    } while (0)

#define ASSERT_GT(val1, val2)                                                                      \
    do {                                                                                           \
        const auto& gtest_lhs = (val1);                                                            \
        const auto& gtest_rhs = (val2);                                                            \
        if (::testing::internal::compare_greater(                                                  \
                gtest_lhs, gtest_rhs, #val1 " > " #val2, __FILE__, __LINE__, true)) {              \
            return;                                                                                \
        }                                                                                          \
    } while (0)

#define EXPECT_GE(val1, val2)                                                                      \
    do {                                                                                           \
        const auto& gtest_lhs = (val1);                                                            \
        const auto& gtest_rhs = (val2);                                                            \
        ::testing::internal::compare_greater_equal(                                                \
            gtest_lhs, gtest_rhs, #val1 " >= " #val2, __FILE__, __LINE__, false);                  \
    } while (0)

#define ASSERT_GE(val1, val2)                                                                      \
    do {                                                                                           \
        const auto& gtest_lhs = (val1);                                                            \
        const auto& gtest_rhs = (val2);                                                            \
        if (::testing::internal::compare_greater_equal(                                            \
                gtest_lhs, gtest_rhs, #val1 " >= " #val2, __FILE__, __LINE__, true)) {             \
            return;                                                                                \
        }                                                                                          \
    } while (0)

#define EXPECT_NO_THROW(statement)                                                                 \
    do {                                                                                           \
        try {                                                                                      \
            statement;                                                                             \
        } catch (const std::exception& ex) {                                                       \
            ::testing::internal::report_failure(                                                   \
                true, "EXPECT_NO_THROW(" #statement ")", __FILE__, __LINE__, false, ex.what());    \
        } catch (...) {                                                                            \
            ::testing::internal::report_failure(true,                                              \
                                                "EXPECT_NO_THROW(" #statement ")",                 \
                                                __FILE__,                                          \
                                                __LINE__,                                          \
                                                false,                                             \
                                                "non-std exception");                              \
        }                                                                                          \
    } while (0)

#define ASSERT_NO_THROW(statement)                                                                 \
    do {                                                                                           \
        try {                                                                                      \
            statement;                                                                             \
        } catch (const std::exception& ex) {                                                       \
            if (::testing::internal::report_failure(true,                                          \
                                                    "ASSERT_NO_THROW(" #statement ")",             \
                                                    __FILE__,                                      \
                                                    __LINE__,                                      \
                                                    true,                                          \
                                                    ex.what())) {                                  \
                return;                                                                            \
            }                                                                                      \
        } catch (...) {                                                                            \
            if (::testing::internal::report_failure(true,                                          \
                                                    "ASSERT_NO_THROW(" #statement ")",             \
                                                    __FILE__,                                      \
                                                    __LINE__,                                      \
                                                    true,                                          \
                                                    "non-std exception")) {                        \
                return;                                                                            \
            }                                                                                      \
        }                                                                                          \
    } while (0)

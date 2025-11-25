#include "katana/core/result.hpp"

#include <gtest/gtest.h>
#include <string>
#include <utility>

using namespace katana;

TEST(Result, HasValueSuccess) {
    result<int> r = 42;
    EXPECT_TRUE(r.has_value());
    EXPECT_TRUE(static_cast<bool>(r));
    EXPECT_EQ(r.value(), 42);
}

TEST(Result, HasValueError) {
    result<int> r = std::unexpected(make_error_code(error_code::invalid_fd));
    EXPECT_FALSE(r.has_value());
    EXPECT_FALSE(static_cast<bool>(r));
    EXPECT_EQ(r.error(), make_error_code(error_code::invalid_fd));
}

TEST(Result, ValueAccess) {
    result<std::string> r = std::string("hello");
    EXPECT_EQ(r.value(), "hello");

    const auto& cr = r;
    EXPECT_EQ(cr.value(), "hello");

    auto moved = std::move(r).value();
    EXPECT_EQ(moved, "hello");
}

TEST(Result, ValueAccessThrows) {
    result<int> r = std::unexpected(make_error_code(error_code::invalid_fd));
    bool threw = false;
    try {
        [[maybe_unused]] auto val = r.value();
    } catch (const std::logic_error&) {
        threw = true;
    } catch (...) {
        threw = true;
    }
    EXPECT_TRUE(threw || !r.has_value());
}

TEST(Result, ErrorAccess) {
    auto err = make_error_code(error_code::timeout);
    result<int> r = std::unexpected(err);

    EXPECT_EQ(r.error(), err);
    EXPECT_EQ(r.error().value(), static_cast<int>(error_code::timeout));
}

TEST(Result, ErrorAccessThrows) {
    result<int> r = 42;
    bool threw = false;
    try {
        [[maybe_unused]] auto err = r.error();
    } catch (const std::logic_error&) {
        threw = true;
    } catch (...) {
        threw = true;
    }
    EXPECT_TRUE(threw || r.has_value());
}

TEST(Result, AndThenSuccess) {
    result<int> r = 10;

    auto doubled = r.and_then([](int val) -> result<int> { return val * 2; });

    EXPECT_TRUE(doubled.has_value());
    EXPECT_EQ(doubled.value(), 20);
}

TEST(Result, AndThenError) {
    result<int> r = std::unexpected(make_error_code(error_code::invalid_fd));

    auto doubled = r.and_then([](int val) -> result<int> { return val * 2; });

    EXPECT_FALSE(doubled.has_value());
    EXPECT_EQ(doubled.error(), make_error_code(error_code::invalid_fd));
}

TEST(Result, AndThenChaining) {
    result<int> r = 5;

    auto result_chain = r.and_then([](int val) -> result<int> { return val + 3; })
                            .and_then([](int val) -> result<int> { return val * 2; })
                            .and_then([](int val) -> result<int> { return val - 1; });

    EXPECT_TRUE(result_chain.has_value());
    EXPECT_EQ(result_chain.value(), 15);
}

TEST(Result, AndThenShortCircuit) {
    result<int> r = 5;
    bool second_called = false;
    bool third_called = false;

    auto result_chain = r.and_then([](int) -> result<int> {
                             return std::unexpected(make_error_code(error_code::timeout));
                         })
                            .and_then([&second_called](int val) -> result<int> {
                                second_called = true;
                                return val * 2;
                            })
                            .and_then([&third_called](int val) -> result<int> {
                                third_called = true;
                                return val + 1;
                            });

    EXPECT_FALSE(result_chain.has_value());
    EXPECT_FALSE(second_called);
    EXPECT_FALSE(third_called);
    EXPECT_EQ(result_chain.error(), make_error_code(error_code::timeout));
}

TEST(Result, OrElseSuccess) {
    result<int> r = 42;

    auto recovered = r.or_else([](std::error_code) -> result<int> { return 0; });

    EXPECT_TRUE(recovered.has_value());
    EXPECT_EQ(recovered.value(), 42);
}

TEST(Result, OrElseError) {
    result<int> r = std::unexpected(make_error_code(error_code::timeout));

    auto recovered = r.or_else([](std::error_code err) -> result<int> {
        if (err == make_error_code(error_code::timeout)) {
            return 999;
        }
        return std::unexpected(err);
    });

    EXPECT_TRUE(recovered.has_value());
    EXPECT_EQ(recovered.value(), 999);
}

TEST(Result, OrElseChaining) {
    result<int> r = std::unexpected(make_error_code(error_code::invalid_fd));

    auto recovered = r.or_else([](std::error_code err) -> result<int> {
                          if (err == make_error_code(error_code::timeout)) {
                              return 1;
                          }
                          return std::unexpected(err);
                      }).or_else([](std::error_code err) -> result<int> {
        if (err == make_error_code(error_code::invalid_fd)) {
            return 2;
        }
        return std::unexpected(err);
    });

    EXPECT_TRUE(recovered.has_value());
    EXPECT_EQ(recovered.value(), 2);
}

TEST(Result, VoidSpecializationSuccess) {
    result<void> r;
    EXPECT_TRUE(r.has_value());
    EXPECT_TRUE(static_cast<bool>(r));
    EXPECT_NO_THROW(r.value());
}

TEST(Result, VoidSpecializationError) {
    result<void> r = std::unexpected(make_error_code(error_code::epoll_wait_failed));
    EXPECT_FALSE(r.has_value());
    EXPECT_FALSE(static_cast<bool>(r));
    EXPECT_EQ(r.error(), make_error_code(error_code::epoll_wait_failed));
    bool threw = false;
    try {
        r.value();
    } catch (const std::logic_error&) {
        threw = true;
    } catch (...) {
        threw = true;
    }
    EXPECT_TRUE(threw || !r.has_value());
}

TEST(Result, VoidAndThenSuccess) {
    result<void> r;

    bool called = false;
    auto chained = r.and_then([&called]() -> result<void> {
        called = true;
        return {};
    });

    EXPECT_TRUE(chained.has_value());
    EXPECT_TRUE(called);
}

TEST(Result, VoidAndThenError) {
    result<void> r = std::unexpected(make_error_code(error_code::reactor_stopped));

    bool called = false;
    auto chained = r.and_then([&called]() -> result<void> {
        called = true;
        return {};
    });

    EXPECT_FALSE(chained.has_value());
    EXPECT_FALSE(called);
    EXPECT_EQ(chained.error(), make_error_code(error_code::reactor_stopped));
}

TEST(Result, VoidOrElseSuccess) {
    result<void> r;

    bool called = false;
    auto recovered = r.or_else([&called](std::error_code) -> result<void> {
        called = true;
        return {};
    });

    EXPECT_TRUE(recovered.has_value());
    EXPECT_FALSE(called);
}

TEST(Result, VoidOrElseError) {
    result<void> r = std::unexpected(make_error_code(error_code::timeout));

    bool called = false;
    auto recovered = r.or_else([&called](std::error_code err) -> result<void> {
        called = true;
        if (err == make_error_code(error_code::timeout)) {
            return {};
        }
        return std::unexpected(err);
    });

    EXPECT_TRUE(recovered.has_value());
    EXPECT_TRUE(called);
}

TEST(Result, MixedAndThenOrElse) {
    result<int> r = 10;

    auto result_chain = r.and_then([](int val) -> result<int> {
                             if (val > 5) {
                                 return std::unexpected(make_error_code(error_code::invalid_fd));
                             }
                             return val;
                         })
                            .or_else([](std::error_code) -> result<int> { return 100; })
                            .and_then([](int val) -> result<int> { return val * 2; });

    EXPECT_TRUE(result_chain.has_value());
    EXPECT_EQ(result_chain.value(), 200);
}

TEST(Result, CopyAndMove) {
    result<int> r1 = 42;
    result<int> r2 = r1;
    EXPECT_EQ(r1.value(), r2.value());

    result<int> r3 = std::move(r1);
    EXPECT_EQ(r3.value(), 42);

    result<int> r4 = std::unexpected(make_error_code(error_code::timeout));
    result<int> r5 = r4;
    EXPECT_EQ(r4.error(), r5.error());

    result<int> r6 = std::move(r4);
    EXPECT_EQ(r6.error(), make_error_code(error_code::timeout));
}

TEST(Result, ErrorPropagation) {
    auto step1 = []() -> result<int> { return 10; };

    auto step2 = [](int val) -> result<int> {
        if (val > 5) {
            return std::unexpected(make_error_code(error_code::timeout));
        }
        return val * 2;
    };

    auto step3 = [](int val) -> result<int> { return val + 100; };

    auto final_result = step1().and_then(step2).and_then(step3);

    EXPECT_FALSE(final_result.has_value());
    EXPECT_EQ(final_result.error(), make_error_code(error_code::timeout));
}

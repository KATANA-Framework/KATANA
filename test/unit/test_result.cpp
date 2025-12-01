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

TEST(Result, DereferenceOperator) {
    result<int> r = 42;
    EXPECT_EQ(*r, 42);

    const result<int> cr = 100;
    EXPECT_EQ(*cr, 100);
}

TEST(Result, ArrowOperator) {
    struct test_struct {
        int value = 42;
        int get_value() const { return value; }
    };

    result<test_struct> r = test_struct{10};
    EXPECT_EQ(r->value, 10);
    EXPECT_EQ(r->get_value(), 10);

    const result<test_struct> cr = test_struct{20};
    EXPECT_EQ(cr->value, 20);
    EXPECT_EQ(cr->get_value(), 20);
}

TEST(Result, InPlaceConstruction) {
    struct complex_type {
        int a;
        double b;
        std::string c;

        complex_type(int x, double y, std::string z) : a(x), b(y), c(std::move(z)) {}
    };

    result<complex_type> r(std::in_place, 1, 2.5, "test");
    EXPECT_TRUE(r.has_value());
    EXPECT_EQ(r->a, 1);
    EXPECT_EQ(r->b, 2.5);
    EXPECT_EQ(r->c, "test");
}

TEST(Result, ConstAndThen) {
    const result<int> r = 10;

    auto doubled = r.and_then([](const int& val) -> result<int> { return val * 2; });

    EXPECT_TRUE(doubled.has_value());
    EXPECT_EQ(doubled.value(), 20);
}

TEST(Result, RvalueAndThen) {
    auto get_result = []() -> result<int> { return 10; };

    auto doubled = get_result().and_then([](int&& val) -> result<int> { return val * 2; });

    EXPECT_TRUE(doubled.has_value());
    EXPECT_EQ(doubled.value(), 20);
}

TEST(Result, RvalueOrElse) {
    auto get_error = []() -> result<int> {
        return std::unexpected(make_error_code(error_code::timeout));
    };

    auto recovered = get_error().or_else([](std::error_code&&) -> result<int> { return 100; });

    EXPECT_TRUE(recovered.has_value());
    EXPECT_EQ(recovered.value(), 100);
}

TEST(Result, ErrorRvalueAccess) {
    result<int> r = std::unexpected(make_error_code(error_code::timeout));

    auto err = std::move(r).error();
    EXPECT_EQ(err, make_error_code(error_code::timeout));
}

TEST(Result, ErrorMutableAccess) {
    result<int> r = std::unexpected(make_error_code(error_code::timeout));

    auto& err = r.error();
    EXPECT_EQ(err, make_error_code(error_code::timeout));
}

TEST(Result, VoidConstAndThen) {
    const result<void> r;

    bool called = false;
    auto chained = r.and_then([&called]() -> result<void> {
        called = true;
        return {};
    });

    EXPECT_TRUE(chained.has_value());
    EXPECT_TRUE(called);
}

TEST(Result, VoidRvalueAndThen) {
    auto get_result = []() -> result<void> { return {}; };

    bool called = false;
    auto chained = get_result().and_then([&called]() -> result<void> {
        called = true;
        return {};
    });

    EXPECT_TRUE(chained.has_value());
    EXPECT_TRUE(called);
}

TEST(Result, VoidConstOrElse) {
    const result<void> r = std::unexpected(make_error_code(error_code::timeout));

    bool called = false;
    auto recovered = r.or_else([&called](const std::error_code&) -> result<void> {
        called = true;
        return {};
    });

    EXPECT_TRUE(recovered.has_value());
    EXPECT_TRUE(called);
}

TEST(Result, VoidRvalueOrElse) {
    auto get_error = []() -> result<void> {
        return std::unexpected(make_error_code(error_code::timeout));
    };

    bool called = false;
    auto recovered = get_error().or_else([&called](std::error_code&&) -> result<void> {
        called = true;
        return {};
    });

    EXPECT_TRUE(recovered.has_value());
    EXPECT_TRUE(called);
}

TEST(Result, VoidErrorRvalueAccess) {
    result<void> r = std::unexpected(make_error_code(error_code::timeout));

    auto err = std::move(r).error();
    EXPECT_EQ(err, make_error_code(error_code::timeout));
}

TEST(Result, VoidErrorMutableAccess) {
    result<void> r = std::unexpected(make_error_code(error_code::timeout));

    auto& err = r.error();
    EXPECT_EQ(err, make_error_code(error_code::timeout));
}

TEST(Result, ErrorCategoryName) {
    auto& cat = get_error_category();
    EXPECT_EQ(std::string(cat.name()), "katana");
}

TEST(Result, ErrorCategoryMessages) {
    auto& cat = get_error_category();

    EXPECT_EQ(cat.message(static_cast<int>(error_code::ok)), "success");
    EXPECT_EQ(cat.message(static_cast<int>(error_code::epoll_create_failed)),
              "epoll_create failed");
    EXPECT_EQ(cat.message(static_cast<int>(error_code::epoll_ctl_failed)), "epoll_ctl failed");
    EXPECT_EQ(cat.message(static_cast<int>(error_code::epoll_wait_failed)), "epoll_wait failed");
    EXPECT_EQ(cat.message(static_cast<int>(error_code::invalid_fd)), "invalid file descriptor");
    EXPECT_EQ(cat.message(static_cast<int>(error_code::reactor_stopped)), "reactor is stopped");
    EXPECT_EQ(cat.message(static_cast<int>(error_code::timeout)), "operation timed out");
    EXPECT_EQ(cat.message(999), "unknown error");
}

TEST(Result, ErrorCodeConversion) {
    std::error_code ec = error_code::timeout;
    EXPECT_EQ(ec.value(), static_cast<int>(error_code::timeout));
    EXPECT_EQ(ec.category().name(), std::string("katana"));
}

TEST(Result, UnexpectedDeductionGuide) {
    auto err = make_error_code(error_code::timeout);
    std::unexpected unexp(err);

    EXPECT_EQ(unexp.error(), err);
}

TEST(Result, UnexpectedMoveConstruction) {
    auto err = make_error_code(error_code::timeout);
    std::unexpected unexp1(err);
    std::unexpected unexp2(std::move(err));

    EXPECT_EQ(unexp1.error(), make_error_code(error_code::timeout));
    EXPECT_EQ(unexp2.error(), make_error_code(error_code::timeout));
}

TEST(Result, UnexpectedErrorAccess) {
    auto err = make_error_code(error_code::timeout);
    std::unexpected unexp(err);

    const auto& err_ref = unexp.error();
    EXPECT_EQ(err_ref, err);

    auto& err_mut = unexp.error();
    EXPECT_EQ(err_mut, err);

    auto err_move = std::move(unexp).error();
    EXPECT_EQ(err_move, err);
}

TEST(Result, RvalueDereference) {
    auto get_result = []() -> result<int> { return 42; };

    int val = *get_result();
    EXPECT_EQ(val, 42);
}

TEST(Result, ConstRvalueDereference) {
    const auto get_result = []() -> result<int> { return 42; };

    int val = *get_result();
    EXPECT_EQ(val, 42);
}

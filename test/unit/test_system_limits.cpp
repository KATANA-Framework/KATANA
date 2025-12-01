#include "katana/core/system_limits.hpp"

#include <gtest/gtest.h>

#ifdef __linux__
#include <sys/resource.h>
#endif

using namespace katana;

class SystemLimitsTest : public ::testing::Test {
protected:
#ifdef __linux__
    void SetUp() override {
        // Save current limits
        getrlimit(RLIMIT_NOFILE, &original_limits_);
    }

    void TearDown() override {
        // Restore original limits
        setrlimit(RLIMIT_NOFILE, &original_limits_);
    }

    struct rlimit original_limits_;
#endif
};

TEST_F(SystemLimitsTest, GetMaxFds) {
    auto result = system_limits::get_max_fds();
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(*result, 0);

#ifdef __linux__
    // Should match current rlimit
    struct rlimit rl;
    ASSERT_EQ(getrlimit(RLIMIT_NOFILE, &rl), 0);
    EXPECT_EQ(*result, rl.rlim_cur);
#else
    // On non-Linux, should return default value
    EXPECT_EQ(*result, 1024);
#endif
}

TEST_F(SystemLimitsTest, SetMaxFdsIncrease) {
#ifdef __linux__
    auto current = system_limits::get_max_fds();
    ASSERT_TRUE(current.has_value());

    // Try to increase limit (may require privileges)
    uint64_t new_limit = *current + 100;

    // Check if we can get current hard limit
    struct rlimit rl;
    ASSERT_EQ(getrlimit(RLIMIT_NOFILE, &rl), 0);

    if (new_limit <= rl.rlim_max) {
        auto result = system_limits::set_max_fds(new_limit);
        if (result.has_value()) {
            // Verify it was set
            auto verify = system_limits::get_max_fds();
            ASSERT_TRUE(verify.has_value());
            EXPECT_GE(*verify, *current);
        }
        // If it fails, it's OK - might need privileges
    }
#else
    // On non-Linux, should succeed but do nothing
    auto result = system_limits::set_max_fds(2048);
    EXPECT_TRUE(result.has_value());
#endif
}

TEST_F(SystemLimitsTest, SetMaxFdsDecrease) {
#ifdef __linux__
    auto current = system_limits::get_max_fds();
    ASSERT_TRUE(current.has_value());

    // Decrease limit - should always work
    uint64_t new_limit = std::max(static_cast<uint64_t>(256), *current / 2);

    auto result = system_limits::set_max_fds(new_limit);
    EXPECT_TRUE(result.has_value());

    if (result.has_value()) {
        // Verify it was set
        auto verify = system_limits::get_max_fds();
        ASSERT_TRUE(verify.has_value());
        EXPECT_LE(*verify, *current);
        EXPECT_GE(*verify, new_limit);
    }
#else
    auto result = system_limits::set_max_fds(256);
    EXPECT_TRUE(result.has_value());
#endif
}

TEST_F(SystemLimitsTest, SetMaxFdsSameValue) {
    auto current = system_limits::get_max_fds();
    ASSERT_TRUE(current.has_value());

    // Set to same value - should always work
    auto result = system_limits::set_max_fds(*current);

#ifdef __linux__
    EXPECT_TRUE(result.has_value());

    // Verify it's still the same
    auto verify = system_limits::get_max_fds();
    ASSERT_TRUE(verify.has_value());
    EXPECT_EQ(*verify, *current);
#else
    EXPECT_TRUE(result.has_value());
#endif
}

TEST_F(SystemLimitsTest, SetMaxFdsZero) {
#ifdef __linux__
    // Setting to 0 should either fail or be clamped to minimum
    auto result = system_limits::set_max_fds(0);
    // We don't care if it succeeds or fails, just shouldn't crash
    (void)result;
#else
    auto result = system_limits::set_max_fds(0);
    EXPECT_TRUE(result.has_value());
#endif
}

TEST_F(SystemLimitsTest, SetMaxFdsVeryLarge) {
#ifdef __linux__
    // Try to set a very large limit - likely to fail without root
    uint64_t huge_limit = 1ULL << 30; // 1 billion

    auto result = system_limits::set_max_fds(huge_limit);
    // May succeed or fail depending on privileges and hard limit
    // Just verify it doesn't crash
    (void)result;
#else
    auto result = system_limits::set_max_fds(1ULL << 30);
    EXPECT_TRUE(result.has_value());
#endif
}

TEST_F(SystemLimitsTest, ApplyDefaultConfig) {
    limits_config config;
    EXPECT_EQ(config.max_fds, 65536);

    auto result = system_limits::apply(config);

#ifdef __linux__
    // May succeed or fail depending on privileges
    // Just check it doesn't crash
    (void)result;
#else
    EXPECT_TRUE(result.has_value());
#endif
}

TEST_F(SystemLimitsTest, ApplyCustomConfig) {
    auto current = system_limits::get_max_fds();
    ASSERT_TRUE(current.has_value());

    limits_config config;
    config.max_fds = *current; // Use current value
    config.max_body_size = 5ULL * 1024ULL * 1024ULL;
    config.max_header_size = 4ULL * 1024ULL;

    auto result = system_limits::apply(config);
    EXPECT_TRUE(result.has_value());

    if (result.has_value()) {
        // Verify max_fds was applied
        auto verify = system_limits::get_max_fds();
        ASSERT_TRUE(verify.has_value());
        EXPECT_EQ(*verify, config.max_fds);
    }
}

TEST_F(SystemLimitsTest, ApplySmallConfig) {
    limits_config config;
    config.max_fds = 512;
    config.max_body_size = 1024;
    config.max_header_size = 512;

    auto result = system_limits::apply(config);

#ifdef __linux__
    EXPECT_TRUE(result.has_value());

    if (result.has_value()) {
        auto verify = system_limits::get_max_fds();
        ASSERT_TRUE(verify.has_value());
        EXPECT_GE(*verify, 512);
    }
#else
    EXPECT_TRUE(result.has_value());
#endif
}

TEST_F(SystemLimitsTest, MultipleSetCalls) {
    auto initial = system_limits::get_max_fds();
    ASSERT_TRUE(initial.has_value());

#ifdef __linux__
    // Try multiple sets in sequence
    uint64_t value1 = std::max(static_cast<uint64_t>(256), *initial / 2);
    uint64_t value2 = std::max(static_cast<uint64_t>(512), *initial / 4);

    auto result1 = system_limits::set_max_fds(value1);
    auto result2 = system_limits::set_max_fds(value2);

    // Both should succeed
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result2.has_value());

    // Final value should reflect last set
    auto final = system_limits::get_max_fds();
    ASSERT_TRUE(final.has_value());
#else
    auto result1 = system_limits::set_max_fds(256);
    auto result2 = system_limits::set_max_fds(512);
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result2.has_value());
#endif
}

TEST_F(SystemLimitsTest, LimitsConfigDefaultValues) {
    limits_config config;

    EXPECT_EQ(config.max_fds, 65536);
    EXPECT_EQ(config.max_body_size, 10ULL * 1024ULL * 1024ULL);
    EXPECT_EQ(config.max_header_size, 8ULL * 1024ULL);
}

TEST_F(SystemLimitsTest, SetAndRestore) {
    auto original = system_limits::get_max_fds();
    ASSERT_TRUE(original.has_value());

#ifdef __linux__
    // Set to different value
    uint64_t temp_limit = std::max(static_cast<uint64_t>(256), *original / 2);
    auto set_result = system_limits::set_max_fds(temp_limit);

    if (set_result.has_value()) {
        // Restore original
        auto restore_result = system_limits::set_max_fds(*original);
        EXPECT_TRUE(restore_result.has_value());

        // Verify restored
        auto final = system_limits::get_max_fds();
        ASSERT_TRUE(final.has_value());
        EXPECT_EQ(*final, *original);
    }
#else
    auto set_result = system_limits::set_max_fds(256);
    EXPECT_TRUE(set_result.has_value());

    auto restore_result = system_limits::set_max_fds(*original);
    EXPECT_TRUE(restore_result.has_value());
#endif
}

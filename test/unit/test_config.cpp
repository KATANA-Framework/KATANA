// Unit tests for katana::config — layered configuration (defaults < file < env < flags) with
// normalized keys, typed accessors and required-key validation.

#include "katana/core/config.hpp"

#include <cstdlib>
#include <fstream>
#include <gtest/gtest.h>

namespace {

using katana::config::config;

TEST(Config, LayerPrecedenceFlagsOverEnvOverFileOverDefaults) {
    const std::string path = std::string(std::tmpnam(nullptr)) + ".conf";
    {
        std::ofstream f(path);
        f << "# a comment\n";
        f << "port = 7000\n";
        f << "db_dsn = \"postgres://from-file\"\n";
        f << "\n";
    }
    ::setenv("KATANA_PORT", "8000", 1);
    ::setenv("KATANA_LOG_LEVEL", "debug", 1);

    const char* argv[] = {"prog", "--port=9000", "--workers", "8"};
    config cfg;
    cfg.defaults({{"port", "8080"}, {"workers", "4"}})
        .from_file(path)
        .from_env("KATANA")
        .from_args(4, argv);

    EXPECT_EQ(cfg.get_u16("port", 0), 9000);                 // flag beats env/file/default
    EXPECT_EQ(cfg.get_int("workers", 0), 8);                 // flag beats default
    EXPECT_EQ(cfg.get_or("log_level", ""), "debug");         // from env
    EXPECT_EQ(cfg.get_or("db_dsn", ""), "postgres://from-file"); // from file, quotes stripped

    ::unsetenv("KATANA_PORT");
    ::unsetenv("KATANA_LOG_LEVEL");
    std::remove(path.c_str());
}

TEST(Config, NormalizesKeysAcrossSources) {
    const char* argv[] = {"prog", "--db-dsn", "x", "--Max-Conns=10"};
    config cfg;
    cfg.from_args(4, argv);
    EXPECT_EQ(cfg.get_or("db_dsn", ""), "x");      // '-' -> '_'
    EXPECT_EQ(cfg.get_int("max_conns", 0), 10);    // '-' -> '_' and lowercased
}

TEST(Config, EnvPrefixIsolation) {
    ::setenv("KATANA_FOO", "1", 1);
    ::setenv("UNRELATED_BAR", "2", 1);
    config cfg;
    cfg.from_env("KATANA");
    EXPECT_TRUE(cfg.contains("foo"));
    EXPECT_FALSE(cfg.contains("unrelated_bar"));
    EXPECT_FALSE(cfg.contains("bar"));
    ::unsetenv("KATANA_FOO");
    ::unsetenv("UNRELATED_BAR");
}

TEST(Config, BareBooleanFlagAndGreedyValue) {
    const char* argv[] = {"prog", "--workers", "8", "--verbose"};
    config cfg;
    cfg.from_args(4, argv);
    EXPECT_EQ(cfg.get_int("workers", 0), 8); // value flag consumes next token
    EXPECT_TRUE(cfg.get_bool("verbose", false)); // bare flag at end -> "true"
}

TEST(Config, TypedAccessorsFallBackOnMissingOrUnparseable) {
    config cfg;
    cfg.set("not_a_number", "abc").set("huge", "70000");
    EXPECT_EQ(cfg.get_int("not_a_number", 42), 42);   // unparseable -> fallback
    EXPECT_EQ(cfg.get_int("absent", 7), 7);           // missing -> fallback
    EXPECT_EQ(cfg.get_u16("huge", 1234), 1234);       // out of u16 range -> fallback
    EXPECT_EQ(cfg.get_bool("absent", true), true);
}

TEST(Config, BoolParsingAcceptsCommonSpellings) {
    config cfg;
    cfg.set("a", "on").set("b", "No").set("c", "TRUE").set("d", "0");
    EXPECT_TRUE(cfg.get_bool("a", false));
    EXPECT_FALSE(cfg.get_bool("b", true));
    EXPECT_TRUE(cfg.get_bool("c", false));
    EXPECT_FALSE(cfg.get_bool("d", true));
}

TEST(Config, ValidateReportsMissingRequiredAndLoadErrors) {
    config cfg;
    cfg.from_file("/no/such/config/file.conf"); // load error
    cfg.set("present", "x");
    auto problems = cfg.validate({"present", "absent_one", "absent_two"});
    // 1 load error + 2 missing = 3
    EXPECT_EQ(problems.size(), 3u);
}

} // namespace

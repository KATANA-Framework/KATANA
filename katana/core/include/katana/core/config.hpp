#pragma once

// Layered configuration: merge values from (in increasing precedence) programmatic defaults,
// a config file, environment variables, and command-line flags into one flat key→string map,
// then read them back with typed, validated accessors.
//
//   katana::config::config cfg;
//   cfg.defaults({{"port", "8080"}, {"workers", "4"}})
//      .from_file("service.conf")          // key = value lines, '#' comments
//      .from_env("KATANA")                 // KATANA_PORT -> port
//      .from_args(argc, argv);             // --port 9090 / --port=9090 / --verbose
//
//   uint16_t port = cfg.get_u16("port", 8080);
//   if (auto errs = cfg.validate({"db_dsn"}); !errs.empty()) { /* report and exit */ }
//
// Keys are normalized to lowercase snake_case: file/flag separators '-' become '_', and an
// env var like KATANA_DB_DSN maps to the key `db_dsn`. This keeps the three external sources
// addressing the same key space without ambiguity.

#include <charconv>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <unistd.h> // POSIX `environ`

namespace katana::config {

namespace detail {

inline std::string normalize_key(std::string_view raw) {
    std::string key;
    key.reserve(raw.size());
    for (char c : raw) {
        key.push_back(c == '-' ? '_' : static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    return key;
}

inline std::string_view trim(std::string_view s) {
    const auto not_space = [](char c) { return std::isspace(static_cast<unsigned char>(c)) == 0; };
    while (!s.empty() && !not_space(s.front())) {
        s.remove_prefix(1);
    }
    while (!s.empty() && !not_space(s.back())) {
        s.remove_suffix(1);
    }
    return s;
}

// Strip one layer of matching surrounding quotes, if present.
inline std::string_view unquote(std::string_view s) {
    if (s.size() >= 2 && (s.front() == '"' || s.front() == '\'') && s.back() == s.front()) {
        return s.substr(1, s.size() - 2);
    }
    return s;
}

} // namespace detail

class config {
public:
    // Seed programmatic defaults (lowest precedence). Does not overwrite values already set by
    // a higher-precedence source applied earlier — but in practice call this first.
    config& defaults(std::initializer_list<std::pair<std::string_view, std::string_view>> kv) {
        for (const auto& [k, v] : kv) {
            values_[detail::normalize_key(k)] = std::string(v);
        }
        return *this;
    }

    // Set/override a single value programmatically.
    config& set(std::string_view key, std::string_view value) {
        values_[detail::normalize_key(key)] = std::string(value);
        return *this;
    }

    // Load `key = value` lines from a file. Lines starting with '#' (after trimming) and blank
    // lines are ignored; values may be quoted. A missing file is recorded as a load error but
    // does not throw — call `errors()` to inspect. Existing keys are overwritten.
    config& from_file(std::string_view path) {
        std::ifstream in{std::string(path)};
        if (!in) {
            load_errors_.push_back("config file not readable: " + std::string(path));
            return *this;
        }
        std::string line;
        size_t line_no = 0;
        while (std::getline(in, line)) {
            ++line_no;
            std::string_view view = detail::trim(line);
            if (view.empty() || view.front() == '#') {
                continue;
            }
            const size_t eq = view.find('=');
            if (eq == std::string_view::npos) {
                load_errors_.push_back("config file " + std::string(path) + ":" +
                                       std::to_string(line_no) + ": expected key = value");
                continue;
            }
            const std::string_view key = detail::trim(view.substr(0, eq));
            const std::string_view value = detail::unquote(detail::trim(view.substr(eq + 1)));
            if (key.empty()) {
                load_errors_.push_back("config file " + std::string(path) + ":" +
                                       std::to_string(line_no) + ": empty key");
                continue;
            }
            values_[detail::normalize_key(key)] = std::string(value);
        }
        return *this;
    }

    // Import environment variables. With a prefix (e.g. "KATANA"), only `PREFIX_*` vars are
    // imported and the prefix (plus its underscore) is stripped: KATANA_DB_DSN -> db_dsn. With
    // an empty prefix every variable is imported, lowercased.
    config& from_env(std::string_view prefix = "") {
        std::string want;
        if (!prefix.empty()) {
            want = std::string(prefix);
            want.push_back('_');
        }
        for (char** env = environ; env != nullptr && *env != nullptr; ++env) {
            std::string_view entry(*env);
            const size_t eq = entry.find('=');
            if (eq == std::string_view::npos) {
                continue;
            }
            std::string_view name = entry.substr(0, eq);
            const std::string_view value = entry.substr(eq + 1);
            if (!want.empty()) {
                if (name.size() <= want.size() || name.substr(0, want.size()) != want) {
                    continue;
                }
                name.remove_prefix(want.size());
            }
            values_[detail::normalize_key(name)] = std::string(value);
        }
        return *this;
    }

    // Parse command-line flags (highest precedence): `--key=value`, `--key value`, or a bare
    // `--flag` (set to "true"). Non-flag arguments are ignored. Keys are normalized, so
    // `--db-dsn` sets `db_dsn`. Note: `--key value` is greedy — a bare boolean flag must be
    // followed by another `--flag` or end-of-args (else the following token is taken as its
    // value). Config-driven services take no positionals, so this is unambiguous in practice;
    // when in doubt use the explicit `--flag=true` form.
    config& from_args(int argc, const char* const* argv) {
        for (int i = 1; i < argc; ++i) {
            std::string_view arg(argv[i]);
            if (!arg.starts_with("--")) {
                continue;
            }
            arg.remove_prefix(2);
            if (arg.empty()) {
                continue; // bare "--" terminator
            }
            const size_t eq = arg.find('=');
            if (eq != std::string_view::npos) {
                values_[detail::normalize_key(arg.substr(0, eq))] = std::string(arg.substr(eq + 1));
                continue;
            }
            // `--key value` if the next arg isn't itself a flag, else a bare boolean flag.
            if (i + 1 < argc && !std::string_view(argv[i + 1]).starts_with("--")) {
                values_[detail::normalize_key(arg)] = std::string(argv[++i]);
            } else {
                values_[detail::normalize_key(arg)] = "true";
            }
        }
        return *this;
    }

    // --- Typed accessors -------------------------------------------------------------------

    [[nodiscard]] bool contains(std::string_view key) const {
        return values_.find(detail::normalize_key(key)) != values_.end();
    }

    [[nodiscard]] std::optional<std::string_view> get(std::string_view key) const {
        auto it = values_.find(detail::normalize_key(key));
        if (it == values_.end()) {
            return std::nullopt;
        }
        return std::string_view(it->second);
    }

    [[nodiscard]] std::string_view get_or(std::string_view key, std::string_view fallback) const {
        auto v = get(key);
        return v ? *v : fallback;
    }

    [[nodiscard]] int64_t get_int(std::string_view key, int64_t fallback) const {
        if (auto parsed = parse_int(key)) {
            return *parsed;
        }
        return fallback;
    }

    [[nodiscard]] uint16_t get_u16(std::string_view key, uint16_t fallback) const {
        auto parsed = parse_int(key);
        if (!parsed || *parsed < 0 || *parsed > 65535) {
            return fallback;
        }
        return static_cast<uint16_t>(*parsed);
    }

    [[nodiscard]] bool get_bool(std::string_view key, bool fallback) const {
        auto v = get(key);
        if (!v) {
            return fallback;
        }
        if (auto b = parse_bool(*v)) {
            return *b;
        }
        return fallback;
    }

    // --- Validation ------------------------------------------------------------------------

    // Errors accumulated while loading sources (unreadable file, malformed lines).
    [[nodiscard]] const std::vector<std::string>& errors() const { return load_errors_; }

    // Return one message per required key that is missing or empty, combined with any load
    // errors. Empty result means the configuration is usable.
    [[nodiscard]] std::vector<std::string>
    validate(std::initializer_list<std::string_view> required) const {
        std::vector<std::string> problems = load_errors_;
        for (std::string_view key : required) {
            auto v = get(key);
            if (!v || v->empty()) {
                problems.push_back("missing required config key: " + detail::normalize_key(key));
            }
        }
        return problems;
    }

    // Flat view of every resolved key (sorted), for diagnostics / a `--dump-config` flag.
    [[nodiscard]] const std::map<std::string, std::string, std::less<>>& entries() const {
        return values_;
    }

private:
    [[nodiscard]] std::optional<int64_t> parse_int(std::string_view key) const {
        auto v = get(key);
        if (!v) {
            return std::nullopt;
        }
        std::string_view s = detail::trim(*v);
        int64_t out = 0;
        const char* begin = s.data();
        const char* end = s.data() + s.size();
        auto [ptr, ec] = std::from_chars(begin, end, out);
        if (ec != std::errc{} || ptr != end) {
            return std::nullopt;
        }
        return out;
    }

    [[nodiscard]] static std::optional<bool> parse_bool(std::string_view raw) {
        std::string lowered;
        lowered.reserve(raw.size());
        for (char c : raw) {
            lowered.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        }
        if (lowered == "1" || lowered == "true" || lowered == "yes" || lowered == "on") {
            return true;
        }
        if (lowered == "0" || lowered == "false" || lowered == "no" || lowered == "off") {
            return false;
        }
        return std::nullopt;
    }

    std::map<std::string, std::string, std::less<>> values_;
    std::vector<std::string> load_errors_;
};

} // namespace katana::config

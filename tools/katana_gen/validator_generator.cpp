#include "generator.hpp"

#include <cctype>
#include <cmath>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_set>

namespace katana_gen {
namespace {

// ============================================================
// Pattern Analysis Helpers
// ============================================================

// Attempts to generate a hand-coded validator for simple regex patterns.
// Returns true if it generated inline code, false if the caller should
// fall back to std::regex.
bool try_generate_handcoded_pattern(std::ostream& out,
                                    std::string_view prop_name,
                                    bool is_optional,
                                    const std::string& obj_prefix,
                                    const std::string& deref_prefix,
                                    std::string_view pattern) {
    // Pattern: ^[a-zA-Z0-9]+$  ->  loop with std::isalnum
    if (pattern == "^[a-zA-Z0-9]+$") {
        std::string val_expr = is_optional ? ("*" + obj_prefix) : obj_prefix;
        std::string empty_check = is_optional ? (obj_prefix + " && !" + obj_prefix + "->empty()")
                                              : ("!" + obj_prefix + ".empty()");
        out << "    if (" << empty_check << ") {\n";
        out << "        const auto& pv_ = " << val_expr << ";\n";
        out << "        bool pattern_ok_ = true;\n";
        out << "        for (unsigned char ch_ : pv_) {\n";
        out << "            if (!std::isalnum(ch_)) { pattern_ok_ = false; break; }\n";
        out << "        }\n";
        out << "        if (!pattern_ok_) {\n";
        out << "            return validation_error{\"" << prop_name
            << "\", validation_error_code::pattern_mismatch};\n";
        out << "        }\n";
        out << "    }\n";
        return true;
    }

    // Pattern: ^[a-zA-Z0-9_-]+$  ->  loop checking alnum, underscore, hyphen
    if (pattern == "^[a-zA-Z0-9_-]+$" || pattern == "^[a-zA-Z0-9_\\-]+$") {
        std::string val_expr = is_optional ? ("*" + obj_prefix) : obj_prefix;
        std::string empty_check = is_optional ? (obj_prefix + " && !" + obj_prefix + "->empty()")
                                              : ("!" + obj_prefix + ".empty()");
        out << "    if (" << empty_check << ") {\n";
        out << "        const auto& pv_ = " << val_expr << ";\n";
        out << "        bool pattern_ok_ = true;\n";
        out << "        for (unsigned char ch_ : pv_) {\n";
        out << "            if (!std::isalnum(ch_) && ch_ != '_' && ch_ != '-') { pattern_ok_ = "
               "false; break; }\n";
        out << "        }\n";
        out << "        if (!pattern_ok_) {\n";
        out << "            return validation_error{\"" << prop_name
            << "\", validation_error_code::pattern_mismatch};\n";
        out << "        }\n";
        out << "    }\n";
        return true;
    }

    // Pattern: ^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$  ->  already have is_valid_email
    if (pattern == "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$" ||
        pattern == "^[a-zA-Z0-9._%+\\-]+@[a-zA-Z0-9.\\-]+\\.[a-zA-Z]{2,}$") {
        out << "    if ("
            << (is_optional ? obj_prefix + " && !" + obj_prefix + "->empty() && !is_valid_email(" +
                                  deref_prefix + ")"
                            : "!" + obj_prefix + ".empty() && !is_valid_email(" + obj_prefix + ")")
            << ") {\n";
        out << "        return validation_error{\"" << prop_name
            << "\", validation_error_code::pattern_mismatch};\n";
        out << "    }\n";
        return true;
    }

    // Pattern: ^[a-zA-Z]+$  ->  loop with std::isalpha
    if (pattern == "^[a-zA-Z]+$") {
        std::string val_expr = is_optional ? ("*" + obj_prefix) : obj_prefix;
        std::string empty_check = is_optional ? (obj_prefix + " && !" + obj_prefix + "->empty()")
                                              : ("!" + obj_prefix + ".empty()");
        out << "    if (" << empty_check << ") {\n";
        out << "        const auto& pv_ = " << val_expr << ";\n";
        out << "        bool pattern_ok_ = true;\n";
        out << "        for (unsigned char ch_ : pv_) {\n";
        out << "            if (!std::isalpha(ch_)) { pattern_ok_ = false; break; }\n";
        out << "        }\n";
        out << "        if (!pattern_ok_) {\n";
        out << "            return validation_error{\"" << prop_name
            << "\", validation_error_code::pattern_mismatch};\n";
        out << "        }\n";
        out << "    }\n";
        return true;
    }

    // Pattern: ^[0-9]+$  ->  loop with std::isdigit
    if (pattern == "^[0-9]+$") {
        std::string val_expr = is_optional ? ("*" + obj_prefix) : obj_prefix;
        std::string empty_check = is_optional ? (obj_prefix + " && !" + obj_prefix + "->empty()")
                                              : ("!" + obj_prefix + ".empty()");
        out << "    if (" << empty_check << ") {\n";
        out << "        const auto& pv_ = " << val_expr << ";\n";
        out << "        bool pattern_ok_ = true;\n";
        out << "        for (unsigned char ch_ : pv_) {\n";
        out << "            if (!std::isdigit(ch_)) { pattern_ok_ = false; break; }\n";
        out << "        }\n";
        out << "        if (!pattern_ok_) {\n";
        out << "            return validation_error{\"" << prop_name
            << "\", validation_error_code::pattern_mismatch};\n";
        out << "        }\n";
        out << "    }\n";
        return true;
    }

    // Not a simple pattern - fall through to std::regex
    return false;
}

// Helper to generate uniqueItems validation with sort+adjacent_find for small arrays
// and hash set fallback for large arrays. Boolean arrays keep their efficient pattern.
void generate_unique_items_check(std::ostream& out,
                                 const document& doc,
                                 std::string_view prop_name,
                                 const std::string& arr_expr,
                                 const katana::openapi::schema* item_schema) {
    using katana::openapi::schema_kind;
    if (!item_schema) {
        return;
    }

    const auto item_kind = item_schema->kind;
    const bool item_is_enum = item_kind == schema_kind::string && !item_schema->enum_values.empty();

    if (item_kind == schema_kind::boolean) {
        // Boolean: keep efficient seen_true/seen_false pattern
        out << "        bool seen_true = false, seen_false = false;\n";
        out << "        for (const auto& v : " << arr_expr << ") {\n";
        out << "            if (v) {\n";
        out << "                if (seen_true) return validation_error{\"" << prop_name
            << "\", validation_error_code::array_items_not_unique};\n";
        out << "                seen_true = true;\n";
        out << "            } else {\n";
        out << "                if (seen_false) return validation_error{\"" << prop_name
            << "\", validation_error_code::array_items_not_unique};\n";
        out << "                seen_false = true;\n";
        out << "            }\n";
        out << "        }\n";
        return;
    }

    // Determine the C++ type for the set and sort
    std::string cpp_type;
    if (item_is_enum) {
        cpp_type = schema_identifier(doc, item_schema) + "_enum";
    } else if (item_kind == schema_kind::string) {
        cpp_type = "std::string_view";
    } else if (item_kind == schema_kind::integer) {
        cpp_type = "int64_t";
    } else if (item_kind == schema_kind::number) {
        cpp_type = "double";
    } else {
        // Unknown item type: fall back to O(n^2) comparison (no sort available)
        out << "        for (size_t i = 0; i < " << arr_expr << ".size(); ++i) {\n";
        out << "            for (size_t j = i + 1; j < " << arr_expr << ".size(); ++j) {\n";
        out << "                if (" << arr_expr << "[i] == " << arr_expr << "[j]) {\n";
        out << "                    return validation_error{\"" << prop_name
            << "\", validation_error_code::array_items_not_unique};\n";
        out << "                }\n";
        out << "            }\n";
        out << "        }\n";
        return;
    }

    // Runtime size check: sort+adjacent_find for small, hash set for large
    out << "        if (" << arr_expr << ".size() <= 64) {\n";
    out << "            // Small array: sort a copy (stack-friendly, no heap)\n";
    if (item_kind == schema_kind::string && !item_is_enum) {
        out << "            std::vector<std::string_view> tmp_(" << arr_expr << ".begin(), "
            << arr_expr << ".end());\n";
    } else {
        out << "            auto tmp_ = " << arr_expr << ";\n";
    }
    out << "            std::sort(tmp_.begin(), tmp_.end());\n";
    out << "            if (std::adjacent_find(tmp_.begin(), tmp_.end()) != tmp_.end()) {\n";
    out << "                return validation_error{\"" << prop_name
        << "\", validation_error_code::array_items_not_unique};\n";
    out << "            }\n";
    out << "        } else {\n";
    out << "            // Large array: hash set\n";
    out << "            std::unordered_set<" << cpp_type << "> seen_;\n";
    out << "            for (const auto& v : " << arr_expr << ") {\n";
    out << "                if (!seen_.insert(v).second) {\n";
    out << "                    return validation_error{\"" << prop_name
        << "\", validation_error_code::array_items_not_unique};\n";
    out << "                }\n";
    out << "            }\n";
    out << "        }\n";
}

void generate_validator_for_schema(std::ostream& out,
                                   const document& doc,
                                   const katana::openapi::schema& s) {
    using katana::openapi::schema_kind;

    // Handle top-level arrays (e.g., body: array<number>)
    if (s.kind == schema_kind::array) {
        auto struct_name = schema_identifier(doc, &s);
        out << "[[nodiscard]] inline std::optional<validation_error> validate_" << struct_name
            << "(const " << struct_name << "& arr) {\n";
        // Suppress unused parameter warning when no array constraints
        if (!s.min_items && !s.max_items && !s.unique_items) {
            out << "    (void)arr;\n";
        }
        if (s.min_items) {
            out << "    if (arr.size() < " << *s.min_items
                << ") return validation_error{\"\", validation_error_code::array_too_small, "
                << *s.min_items << "};\n";
        }
        if (s.max_items) {
            out << "    if (arr.size() > " << *s.max_items
                << ") return validation_error{\"\", validation_error_code::array_too_large, "
                << *s.max_items << "};\n";
        }
        if (s.unique_items) {
            out << "    {\n";
            if (s.items) {
                generate_unique_items_check(out, doc, "", "arr", s.items);
            }
            out << "    }\n";
        }
        out << "    return std::nullopt;\n";
        out << "}\n\n";
        return;
    }

    if (s.properties.empty()) {
        return;
    }

    auto struct_name = schema_identifier(doc, &s);

    // Use unified validation_error instead of per-struct error types
    out << "[[nodiscard]] inline std::optional<validation_error> validate_" << struct_name
        << "(const " << struct_name << "& obj) {\n";

    // Check if there's any actual validation logic needed
    bool has_validation = false;
    for (const auto& prop : s.properties) {
        if (!prop.type)
            continue;
        bool is_enum = prop.type->kind == schema_kind::string && !prop.type->enum_values.empty();

        // Check if this property needs any validation
        // Only string/array required fields generate actual validation code
        if (prop.required && !is_enum && prop.type->kind == schema_kind::string) {
            has_validation = true;
            break;
        }
        if (prop.required && prop.type->kind == schema_kind::array && prop.type->min_items &&
            *prop.type->min_items > 0) {
            has_validation = true;
            break;
        }
        if (prop.type->min_length || prop.type->max_length) {
            has_validation = true;
            break;
        }
        if (prop.type->format == "email" || prop.type->format == "uuid" ||
            prop.type->format == "date-time") {
            has_validation = true;
            break;
        }
        if (!prop.type->pattern.empty()) {
            has_validation = true;
            break;
        }
        if (prop.type->minimum || prop.type->maximum) {
            has_validation = true;
            break;
        }
        if (prop.type->exclusive_minimum || prop.type->exclusive_maximum) {
            has_validation = true;
            break;
        }
        if (prop.type->multiple_of) {
            has_validation = true;
            break;
        }
        if (prop.type->min_items || prop.type->max_items) {
            has_validation = true;
            break;
        }
        if (prop.type->unique_items) {
            has_validation = true;
            break;
        }
    }

    if (!has_validation) {
        out << "    (void)obj;\n";
    }

    for (const auto& prop : s.properties) {
        if (!prop.type) {
            continue;
        }
        using katana::openapi::schema_kind;

        const auto prop_name_upper = metadata_constant_identifier(prop.name);
        const auto member_name = property_member_identifier(prop.name);
        const std::string obj_prefix = "obj." + member_name;
        const std::string deref_prefix = "(*obj." + member_name + ")";
        bool is_optional = is_optional_property(prop);

        // Skip required check for enums - they're strongly typed and can't be empty
        bool is_enum = prop.type->kind == schema_kind::string && !prop.type->enum_values.empty();

        if (prop.required && prop.type->kind == schema_kind::string && !is_enum && !is_optional) {
            out << "    if (" << obj_prefix << ".empty()) {\n";
            out << "        return validation_error{\"" << prop.name
                << "\", validation_error_code::required_field_missing};\n";
            out << "    }\n";
        }
        if (prop.required && prop.type->kind == schema_kind::array && prop.type->min_items &&
            *prop.type->min_items > 0 && !is_optional) {
            out << "    if (" << obj_prefix << ".empty()) {\n";
            out << "        return validation_error{\"" << prop.name
                << "\", validation_error_code::required_field_missing};\n";
            out << "    }\n";
        }

        // Skip all string validations for enums (they're strongly typed, validated at parse time)
        if (prop.type->kind == schema_kind::string && !is_enum) {
            if (prop.type->min_length) {
                out << "    if ("
                    << (is_optional ? obj_prefix + " && !" + obj_prefix + "->empty() && " +
                                          obj_prefix + "->size()"
                                    : "!" + obj_prefix + ".empty() && " + obj_prefix + ".size()")
                    << " < " << struct_name << "::metadata::" << prop_name_upper
                    << "_MIN_LENGTH) {\n";
                out << "        return validation_error{\"" << prop.name
                    << "\", validation_error_code::string_too_short, " << struct_name
                    << "::metadata::" << prop_name_upper << "_MIN_LENGTH};\n";
                out << "    }\n";
            }
            if (prop.type->max_length) {
                out << "    if ("
                    << (is_optional ? obj_prefix + " && " + obj_prefix + "->size()"
                                    : obj_prefix + ".size()")
                    << " > " << struct_name << "::metadata::" << prop_name_upper
                    << "_MAX_LENGTH) {\n";
                out << "        return validation_error{\"" << prop.name
                    << "\", validation_error_code::string_too_long, " << struct_name
                    << "::metadata::" << prop_name_upper << "_MAX_LENGTH};\n";
                out << "    }\n";
            }
            if (prop.type->format == "email") {
                out << "    if ("
                    << (is_optional
                            ? obj_prefix + " && !" + obj_prefix + "->empty() && !is_valid_email(" +
                                  deref_prefix + ")"
                            : "!" + obj_prefix + ".empty() && !is_valid_email(" + obj_prefix + ")")
                    << ") {\n";
                out << "        return validation_error{\"" << prop.name
                    << "\", validation_error_code::invalid_email_format};\n";
                out << "    }\n";
            }
            if (prop.type->format == "uuid") {
                out << "    if ("
                    << (is_optional
                            ? obj_prefix + " && !" + obj_prefix + "->empty() && !is_valid_uuid(" +
                                  deref_prefix + ")"
                            : "!" + obj_prefix + ".empty() && !is_valid_uuid(" + obj_prefix + ")")
                    << ") {\n";
                out << "        return validation_error{\"" << prop.name
                    << "\", validation_error_code::invalid_uuid_format};\n";
                out << "    }\n";
            }
            if (prop.type->format == "date-time") {
                out << "    if ("
                    << (is_optional ? obj_prefix + " && !" + obj_prefix +
                                          "->empty() && !is_valid_datetime(" + deref_prefix + ")"
                                    : "!" + obj_prefix + ".empty() && !is_valid_datetime(" +
                                          obj_prefix + ")")
                    << ") {\n";
                out << "        return validation_error{\"" << prop.name
                    << "\", validation_error_code::invalid_datetime_format};\n";
                out << "    }\n";
            }
            // Skip enum validation - enums are strongly typed and validated at parse time
            // (the !is_enum check prevents generating validation for enum types)
            if (!prop.type->pattern.empty()) {
                // Try hand-coded validator for simple patterns first
                if (!try_generate_handcoded_pattern(out,
                                                    prop.name,
                                                    is_optional,
                                                    obj_prefix,
                                                    deref_prefix,
                                                    prop.type->pattern)) {
                    // Complex pattern: fall back to std::regex with static caching
                    out << "    {\n";
                    out << "        static const std::regex re_{\""
                        << escape_cpp_string(prop.type->pattern) << "\"};\n";
                    if (is_optional) {
                        out << "        if (" << obj_prefix << " && !" << obj_prefix
                            << "->empty() && !std::regex_match(" << deref_prefix << ", re_)) {\n";
                    } else {
                        out << "        if (!" << obj_prefix << ".empty() && !std::regex_match("
                            << obj_prefix << ", re_)) {\n";
                    }
                    out << "            return validation_error{\"" << prop.name
                        << "\", validation_error_code::pattern_mismatch};\n";
                    out << "        }\n";
                    out << "    }\n";
                }
            }
        }

        if (prop.type->kind == schema_kind::integer || prop.type->kind == schema_kind::number) {
            if (prop.type->minimum) {
                out << "    if (" << (is_optional ? obj_prefix + " && " : "")
                    << "static_cast<double>(" << (is_optional ? deref_prefix : obj_prefix) << ") < "
                    << struct_name << "::metadata::" << prop_name_upper << "_MINIMUM) {\n";
                out << "        return validation_error{\"" << prop.name
                    << "\", validation_error_code::value_too_small, " << struct_name
                    << "::metadata::" << prop_name_upper << "_MINIMUM};\n";
                out << "    }\n";
            }
            if (prop.type->maximum) {
                out << "    if (" << (is_optional ? obj_prefix + " && " : "")
                    << "static_cast<double>(" << (is_optional ? deref_prefix : obj_prefix) << ") > "
                    << struct_name << "::metadata::" << prop_name_upper << "_MAXIMUM) {\n";
                out << "        return validation_error{\"" << prop.name
                    << "\", validation_error_code::value_too_large, " << struct_name
                    << "::metadata::" << prop_name_upper << "_MAXIMUM};\n";
                out << "    }\n";
            }
            if (prop.type->exclusive_minimum) {
                out << "    if (" << (is_optional ? obj_prefix + " && " : "")
                    << "static_cast<double>(" << (is_optional ? deref_prefix : obj_prefix)
                    << ") <= " << struct_name << "::metadata::" << prop_name_upper
                    << "_EXCLUSIVE_MINIMUM) {\n";
                out << "        return validation_error{\"" << prop.name
                    << "\", validation_error_code::value_below_exclusive_minimum, " << struct_name
                    << "::metadata::" << prop_name_upper << "_EXCLUSIVE_MINIMUM};\n";
                out << "    }\n";
            }
            if (prop.type->exclusive_maximum) {
                out << "    if (" << (is_optional ? obj_prefix + " && " : "")
                    << "static_cast<double>(" << (is_optional ? deref_prefix : obj_prefix)
                    << ") >= " << struct_name << "::metadata::" << prop_name_upper
                    << "_EXCLUSIVE_MAXIMUM) {\n";
                out << "        return validation_error{\"" << prop.name
                    << "\", validation_error_code::value_above_exclusive_maximum, " << struct_name
                    << "::metadata::" << prop_name_upper << "_EXCLUSIVE_MAXIMUM};\n";
                out << "    }\n";
            }
            if (prop.type->multiple_of) {
                double mof = *prop.type->multiple_of;
                bool is_integer_field = (prop.type->kind == schema_kind::integer);
                bool mof_is_integral = (mof == static_cast<double>(static_cast<int64_t>(mof)));

                if (is_integer_field && mof_is_integral) {
                    // Integer field with integral multipleOf: use integer modulo (no floating
                    // point)
                    out << "    if (" << (is_optional ? obj_prefix + " && " : "")
                        << (is_optional ? deref_prefix : obj_prefix) << " % static_cast<int64_t>("
                        << struct_name << "::metadata::" << prop_name_upper
                        << "_MULTIPLE_OF) != 0) {\n";
                } else {
                    // Number field or fractional multipleOf: use std::fmod
                    out << "    if (" << (is_optional ? obj_prefix + " && " : "")
                        << "std::fmod(static_cast<double>("
                        << (is_optional ? deref_prefix : obj_prefix) << "), " << struct_name
                        << "::metadata::" << prop_name_upper << "_MULTIPLE_OF) != 0.0) {\n";
                }
                out << "        return validation_error{\"" << prop.name
                    << "\", validation_error_code::value_not_multiple_of, " << struct_name
                    << "::metadata::" << prop_name_upper << "_MULTIPLE_OF};\n";
                out << "    }\n";
            }
        }

        if (prop.type->kind == schema_kind::array) {
            if (prop.type->min_items) {
                out << "    if ("
                    << (is_optional ? obj_prefix + " && !" + obj_prefix + "->empty() && " +
                                          obj_prefix + "->size()"
                                    : "!" + obj_prefix + ".empty() && " + obj_prefix + ".size()")
                    << " < " << struct_name << "::metadata::" << prop_name_upper
                    << "_MIN_ITEMS) {\n";
                out << "        return validation_error{\"" << prop.name
                    << "\", validation_error_code::array_too_small, " << struct_name
                    << "::metadata::" << prop_name_upper << "_MIN_ITEMS};\n";
                out << "    }\n";
            }
            if (prop.type->max_items) {
                out << "    if ("
                    << (is_optional ? obj_prefix + " && " + obj_prefix + "->size()"
                                    : obj_prefix + ".size()")
                    << " > " << struct_name << "::metadata::" << prop_name_upper
                    << "_MAX_ITEMS) {\n";
                out << "        return validation_error{\"" << prop.name
                    << "\", validation_error_code::array_too_large, " << struct_name
                    << "::metadata::" << prop_name_upper << "_MAX_ITEMS};\n";
                out << "    }\n";
            }
            if (prop.type->unique_items) {
                out << "    {\n";
                if (is_optional) {
                    out << "        if (!" << obj_prefix << ") {\n";
                    out << "            // nullable/omitted array: uniqueness check does not "
                           "apply\n";
                    out << "        } else {\n";
                    if (prop.type->items) {
                        generate_unique_items_check(
                            out, doc, prop.name, deref_prefix, prop.type->items);
                    }
                    out << "        }\n";
                } else if (prop.type->items) {
                    generate_unique_items_check(out, doc, prop.name, obj_prefix, prop.type->items);
                }
                out << "    }\n";
            }
        }
    }

    out << "    return std::nullopt;\n";
    out << "}\n\n";
}

} // namespace

std::string generate_validators(const document& doc) {
    std::ostringstream out;
    out << "// Auto-generated validators from OpenAPI specification\n";
    out << "//\n";
    out << "// This file contains:\n";
    out << "//   - Validation functions for all request/response types\n";
    out << "//   - Format validators (email, UUID, date-time, etc.)\n";
    out << "//   - Constraint validators (length, range, pattern, etc.)\n";
    out << "//   - Enum value validators\n";
    out << "//\n";
    out << "// All validators return std::optional<validation_error>:\n";
    out << "//   - std::nullopt on success\n";
    out << "//   - validation_error with field path and error code on failure\n";
    out << "//\n";
    out << "// Validation is automatically called by router bindings before handler execution.\n";
    out << "// Invalid requests return 400 Bad Request with error details.\n";
    out << "//\n";
    out << "#pragma once\n\n";
    out << "#include \"generated_dtos.hpp\"\n";
    out << "#include \"katana/core/validation.hpp\"\n";
    out << "#include \"katana/core/format_validators.hpp\"\n";
    out << "#include <algorithm>\n";
    out << "#include <optional>\n";
    out << "#include <string_view>\n";
    out << "#include <string>\n";
    out << "#include <cmath>\n";
    out << "#include <cctype>\n";
    out << "#include <regex>\n";
    out << "#include <unordered_set>\n";
    out << "#include <vector>\n\n";
    out << "using katana::validation_error;\n";
    out << "using katana::validation_error_code;\n\n";

    // ============================================================
    // Format Validators (from framework)
    // ============================================================
    out << "// ============================================================\n";
    out << "// Format Validators (from framework)\n";
    out << "// ============================================================\n\n";
    out << "using katana::format_validators::is_valid_email;\n";
    out << "using katana::format_validators::is_valid_uuid;\n";
    out << "using katana::format_validators::is_valid_datetime;\n\n";

    // ============================================================
    // Validation Functions
    // ============================================================
    out << "// ============================================================\n";
    out << "// Validation Functions\n";
    out << "// ============================================================\n\n";

    for (const auto& schema : doc.schemas) {
        generate_validator_for_schema(out, doc, schema);
    }

    return out.str();
}

} // namespace katana_gen

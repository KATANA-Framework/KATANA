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

void generate_validator_for_schema(std::ostream& out,
                                   const document& doc,
                                   const katana::openapi::schema& s) {
    using katana::openapi::schema_kind;

    // Handle top-level arrays (e.g., body: array<number>)
    if (s.kind == schema_kind::array) {
        auto struct_name = schema_identifier(doc, &s);
        out << "inline std::optional<validation_error> validate_" << struct_name << "(const "
            << struct_name << "& arr) {\n";
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
                auto item_kind = s.items->kind;
                if (item_kind == schema_kind::string) {
                    out << "        std::unordered_set<std::string_view> seen;\n";
                    out << "        for (const auto& v : arr) {\n";
                    out << "            if (!seen.insert(v).second) {\n";
                    out << "                return validation_error{\"\", "
                           "validation_error_code::array_items_not_unique};\n";
                    out << "            }\n";
                    out << "        }\n";
                } else if (item_kind == schema_kind::integer) {
                    out << "        std::unordered_set<int64_t> seen;\n";
                    out << "        for (const auto& v : arr) {\n";
                    out << "            if (!seen.insert(v).second) {\n";
                    out << "                return validation_error{\"\", "
                           "validation_error_code::array_items_not_unique};\n";
                    out << "            }\n";
                    out << "        }\n";
                } else if (item_kind == schema_kind::number) {
                    out << "        std::unordered_set<double> seen;\n";
                    out << "        for (const auto& v : arr) {\n";
                    out << "            if (!seen.insert(v).second) {\n";
                    out << "                return validation_error{\"\", "
                           "validation_error_code::array_items_not_unique};\n";
                    out << "            }\n";
                    out << "        }\n";
                } else if (item_kind == schema_kind::boolean) {
                    out << "        bool seen_true = false, seen_false = false;\n";
                    out << "        for (const auto& v : arr) {\n";
                    out << "            if (v) {\n";
                    out << "                if (seen_true) return validation_error{\"\", "
                           "validation_error_code::array_items_not_unique};\n";
                    out << "                seen_true = true;\n";
                    out << "            } else {\n";
                    out << "                if (seen_false) return validation_error{\"\", "
                           "validation_error_code::array_items_not_unique};\n";
                    out << "                seen_false = true;\n";
                    out << "            }\n";
                    out << "        }\n";
                }
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
    out << "inline std::optional<validation_error> validate_" << struct_name << "(const "
        << struct_name << "& obj) {\n";

    for (const auto& prop : s.properties) {
        if (!prop.type) {
            continue;
        }
        using katana::openapi::schema_kind;

        std::string prop_name_upper(prop.name.begin(), prop.name.end());
        for (auto& c : prop_name_upper) {
            if (c == '-' || c == ' ')
                c = '_';
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        }
        const std::string prop_name_str(prop.name);
        const std::string obj_prefix = "obj." + prop_name_str;
        const std::string deref_prefix = "*obj." + prop_name_str;
        bool is_optional = prop.type->nullable;

        if (prop.required && prop.type->kind == schema_kind::string) {
            if (is_optional) {
                out << "    if (!obj." << prop.name << ") {\n";
                out << "        return validation_error{\"" << prop.name
                    << "\", validation_error_code::required_field_missing};\n";
                out << "    }\n";
            } else {
                out << "    if (obj." << prop.name << ".empty()) {\n";
                out << "        return validation_error{\"" << prop.name
                    << "\", validation_error_code::required_field_missing};\n";
                out << "    }\n";
            }
        }
        if (prop.required && prop.type->kind == schema_kind::array && prop.type->min_items &&
            *prop.type->min_items > 0) {
            out << "    if ("
                << (is_optional ? "!" + obj_prefix + " || " + obj_prefix + "->empty()"
                                : obj_prefix + ".empty()")
                << ") {\n";
            out << "        return validation_error{\"" << prop.name
                << "\", validation_error_code::required_field_missing};\n";
            out << "    }\n";
        }

        if (prop.type->kind == schema_kind::string) {
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
            if (!prop.type->enum_values.empty()) {
                out << "    {\n";
                out << "        bool valid = false;\n";
                for (const auto& enum_val : prop.type->enum_values) {
                    out << "        if (obj." << prop.name << " == \"" << enum_val
                        << "\") valid = true;\n";
                }
                out << "        if (!valid) {\n";
                out << "            return validation_error{\"" << prop.name
                    << "\", validation_error_code::invalid_enum_value};\n";
                out << "        }\n";
                out << "    }\n";
            }
            if (!prop.type->pattern.empty()) {
                out << "    {\n";
                out << "        static const std::regex re_{\""
                    << escape_cpp_string(prop.type->pattern) << "\"};\n";
                if (is_optional) {
                    out << "        if (obj." << prop.name << " && !obj." << prop.name
                        << "->empty() && !std::regex_match(*obj." << prop.name << ", re_)) {\n";
                } else {
                    out << "        if (!obj." << prop.name << ".empty() && !std::regex_match(obj."
                        << prop.name << ", re_)) {\n";
                }
                out << "            return validation_error{\"" << prop.name
                    << "\", validation_error_code::pattern_mismatch};\n";
                out << "        }\n";
                out << "    }\n";
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
                out << "    if (" << (is_optional ? obj_prefix + " && " : "")
                    << "std::fmod(static_cast<double>(" << (is_optional ? deref_prefix : obj_prefix)
                    << "), " << struct_name << "::metadata::" << prop_name_upper
                    << "_MULTIPLE_OF) != 0.0) {\n";
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
                    out << "            return std::nullopt;\n";
                    out << "        }\n";
                }
                if (prop.type->items) {
                    auto item_kind = prop.type->items->kind;
                    if (item_kind == schema_kind::string) {
                        out << "        std::unordered_set<std::string_view> seen;\n";
                        out << "        for (const auto& v : "
                            << (is_optional ? deref_prefix : obj_prefix) << ") {\n";
                        out << "            if (!seen.insert(v).second) {\n";
                        out << "                return validation_error{\"" << prop.name
                            << "\", validation_error_code::array_items_not_unique};\n";
                        out << "            }\n";
                        out << "        }\n";
                    } else if (item_kind == schema_kind::integer) {
                        out << "        std::unordered_set<int64_t> seen;\n";
                        out << "        for (const auto& v : "
                            << (is_optional ? deref_prefix : obj_prefix) << ") {\n";
                        out << "            if (!seen.insert(v).second) {\n";
                        out << "                return validation_error{\"" << prop.name
                            << "\", validation_error_code::array_items_not_unique};\n";
                        out << "            }\n";
                        out << "        }\n";
                    } else if (item_kind == schema_kind::number) {
                        out << "        std::unordered_set<double> seen;\n";
                        out << "        for (const auto& v : "
                            << (is_optional ? deref_prefix : obj_prefix) << ") {\n";
                        out << "            if (!seen.insert(v).second) {\n";
                        out << "                return validation_error{\"" << prop.name
                            << "\", validation_error_code::array_items_not_unique};\n";
                        out << "            }\n";
                        out << "        }\n";
                    } else if (item_kind == schema_kind::boolean) {
                        out << "        bool seen_true = false, seen_false = false;\n";
                        out << "        for (const auto& v : obj." << prop.name << ") {\n";
                        out << "            if (v) {\n";
                        out << "                if (seen_true) return validation_error{\""
                            << prop.name << "\", validation_error_code::array_items_not_unique};\n";
                        out << "                seen_true = true;\n";
                        out << "            } else {\n";
                        out << "                if (seen_false) return validation_error{\""
                            << prop.name << "\", validation_error_code::array_items_not_unique};\n";
                        out << "                seen_false = true;\n";
                        out << "            }\n";
                        out << "        }\n";
                    } else {
                        out << "        for (size_t i = 0; i < obj." << prop.name
                            << ".size(); ++i) {\n";
                        out << "            for (size_t j = i + 1; j < obj." << prop.name
                            << ".size(); ++j) {\n";
                        out << "                if (obj." << prop.name << "[i] == obj." << prop.name
                            << "[j]) {\n";
                        out << "                    return validation_error{\"" << prop.name
                            << "\", validation_error_code::array_items_not_unique};\n";
                        out << "                }\n";
                        out << "            }\n";
                        out << "        }\n";
                    }
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
    out << "#pragma once\n\n";
    out << "#include \"generated_dtos.hpp\"\n";
    out << "#include \"katana/core/validation.hpp\"\n";
    out << "#include <optional>\n";
    out << "#include <string_view>\n";
    out << "#include <string>\n";
    out << "#include <cmath>\n";
    out << "#include <cctype>\n\n";
    out << "#include <regex>\n";
    out << "#include <unordered_set>\n\n";
    out << "using katana::validation_error;\n";
    out << "using katana::validation_error_code;\n\n";

    // Provide string conversion locally so generated file contains human-readable messages
    out << "inline constexpr std::string_view to_string(validation_error_code code) noexcept {\n";
    out << "    switch (code) {\n";
    out << "    case validation_error_code::required_field_missing: return \"required field is "
           "missing\";\n";
    out << "    case validation_error_code::invalid_type: return \"invalid type\";\n";
    out << "    case validation_error_code::string_too_short: return \"string too short\";\n";
    out << "    case validation_error_code::string_too_long: return \"string too long\";\n";
    out << "    case validation_error_code::invalid_email_format: return \"invalid email "
           "format\";\n";
    out << "    case validation_error_code::invalid_uuid_format: return \"invalid uuid format\";\n";
    out << "    case validation_error_code::invalid_datetime_format: return \"invalid date-time "
           "format\";\n";
    out << "    case validation_error_code::invalid_enum_value: return \"invalid enum value\";\n";
    out << "    case validation_error_code::pattern_mismatch: return \"pattern mismatch\";\n";
    out << "    case validation_error_code::value_too_small: return \"value too small\";\n";
    out << "    case validation_error_code::value_too_large: return \"value too large\";\n";
    out << "    case validation_error_code::value_below_exclusive_minimum: return \"value must be "
           "greater than minimum\";\n";
    out << "    case validation_error_code::value_above_exclusive_maximum: return \"value must be "
           "less than maximum\";\n";
    out << "    case validation_error_code::value_not_multiple_of: return \"value must be multiple "
           "of\";\n";
    out << "    case validation_error_code::array_too_small: return \"array too small\";\n";
    out << "    case validation_error_code::array_too_large: return \"array too large\";\n";
    out << "    case validation_error_code::array_items_not_unique: return \"array items must be "
           "unique\";\n";
    out << "    }\n";
    out << "    return \"unknown error\";\n";
    out << "}\n\n";

    out << "inline bool is_valid_email(std::string_view v) {\n";
    out << "    auto at = v.find('@');\n";
    out << "    if (at == std::string_view::npos || at == 0 || at + 1 >= v.size()) return false;\n";
    out << "    auto domain = v.substr(at + 1);\n";
    out << "    auto dot = domain.find('.');\n";
    out << "    if (dot == std::string_view::npos || dot == 0 || dot + 1 >= domain.size()) return "
           "false;\n";
    out << "    return true;\n";
    out << "}\n\n";

    out << "inline bool is_valid_uuid(std::string_view v) {\n";
    out << "    if (v.size() != 36) return false;\n";
    out << "    auto is_hex = [](char c) { return std::isxdigit(static_cast<unsigned char>(c)) != "
           "0; };\n";
    out << "    for (size_t i = 0; i < v.size(); ++i) {\n";
    out << "        if (i == 8 || i == 13 || i == 18 || i == 23) {\n";
    out << "            if (v[i] != '-') return false;\n";
    out << "        } else if (!is_hex(v[i])) {\n";
    out << "            return false;\n";
    out << "        }\n";
    out << "    }\n";
    out << "    return true;\n";
    out << "}\n\n";

    out << "inline bool is_valid_datetime(std::string_view v) {\n";
    out << "    auto is_digit = [](char c) { return std::isdigit(static_cast<unsigned char>(c)) != "
           "0; };\n";
    out << "    if (v.size() < 20) return false;\n";
    out << "    for (size_t i : {0u, 1u, 2u, 3u, 5u, 6u, 8u, 9u, 11u, 12u, 14u, 15u, 17u, 18u}) "
           "{\n";
    out << "        if (!is_digit(v[i])) return false;\n";
    out << "    }\n";
    out << "    if (v[4] != '-' || v[7] != '-' || v[10] != 'T' || v[13] != ':' || v[16] != ':') "
           "return false;\n";
    out << "    size_t pos = 19;\n";
    out << "    if (pos < v.size() && v[pos] == '.') {\n";
    out << "        ++pos;\n";
    out << "        if (pos >= v.size()) return false;\n";
    out << "        while (pos < v.size() && is_digit(v[pos])) ++pos;\n";
    out << "    }\n";
    out << "    if (pos >= v.size()) return false;\n";
    out << "    if (v[pos] == 'Z') return pos + 1 == v.size();\n";
    out << "    if (v[pos] == '+' || v[pos] == '-') {\n";
    out << "        if (pos + 5 >= v.size()) return false;\n";
    out << "        if (!is_digit(v[pos + 1]) || !is_digit(v[pos + 2])) return false;\n";
    out << "        if (v[pos + 3] != ':') return false;\n";
    out << "        if (!is_digit(v[pos + 4]) || !is_digit(v[pos + 5])) return false;\n";
    out << "        return pos + 6 == v.size();\n";
    out << "    }\n";
    out << "    return false;\n";
    out << "}\n\n";

    for (const auto& schema : doc.schemas) {
        generate_validator_for_schema(out, doc, schema);
    }

    return out.str();
}

} // namespace katana_gen

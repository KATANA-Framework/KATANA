// Row→DTO bridge generator. For each generated `<Name>Row` whose field set exactly matches an
// OpenAPI DTO (same normalized field names, compatible scalar/string types, all DTO fields required
// scalars/strings), it emits a `to_<Dto>(row, arena)` converter. The reverse (DTO→Row) direction
// has no consumer by construction — query inputs are Params structs, not Rows — so it is not
// emitted. Anything that is ambiguous or only a partial match is skipped with a warning — the
// bridge never emits a mapping it isn't sure about. Targets pmr DTOs (the default), whose fields
// are arena_string<> / scalars constructed from an arena.

#include "katana_gen/generator.hpp"
#include "katana_gen/sql_codegen.hpp"

#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace katana_gen {

namespace {

using katana::openapi::schema;
using katana::openapi::schema_kind;

// Field type buckets that can be bridged. OTHER means "not bridgeable" (nested object, non-string
// array, enum, …) and makes the whole match ineligible. STRING_ARRAY bridges text[] ↔ array<string>.
enum class field_kind { integer, number, boolean, string, string_array, other };

field_kind row_column_kind(const std::string& cpp_type) {
    if (cpp_type == "int64_t") {
        return field_kind::integer;
    }
    if (cpp_type == "double") {
        return field_kind::number;
    }
    if (cpp_type == "bool") {
        return field_kind::boolean;
    }
    if (cpp_type == "std::string") {
        return field_kind::string;
    }
    if (cpp_type == "std::vector<std::string>") {
        return field_kind::string_array; // text[] ↔ array<string>
    }
    return field_kind::other;
}

field_kind dto_property_kind(const schema* s) {
    if (s == nullptr) {
        return field_kind::other;
    }
    switch (s->kind) {
    case schema_kind::integer:
        return field_kind::integer;
    case schema_kind::number:
        return field_kind::number;
    case schema_kind::boolean:
        return field_kind::boolean;
    case schema_kind::string:
        return s->enum_values.empty() ? field_kind::string : field_kind::other; // enums: skip
    case schema_kind::array:
        // array<string> (non-enum items) bridges to a text[] row column.
        return (s->items != nullptr && s->items->kind == schema_kind::string &&
                s->items->enum_values.empty())
                   ? field_kind::string_array
                   : field_kind::other;
    default:
        return field_kind::other;
    }
}

// Normalize a field name for matching: lowercase, drop every non-alphanumeric char. So `price_cents`
// and `priceCents` both become `pricecents`.
std::string normalize(std::string_view name) {
    std::string out;
    out.reserve(name.size());
    for (char c : name) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        }
    }
    return out;
}

std::string to_pascal_case(std::string_view sv) {
    std::string out;
    bool upper = true;
    for (char c : sv) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            out.push_back(upper ? static_cast<char>(std::toupper(static_cast<unsigned char>(c)))
                                : static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
            upper = false;
        } else {
            upper = true;
        }
    }
    return out.empty() ? std::string("Query") : out;
}

std::string row_type_name(const sql_query& query) {
    return to_pascal_case(query.name) + "Row";
}

std::string zero_literal(field_kind k) {
    switch (k) {
    case field_kind::integer:
        return "0";
    case field_kind::number:
        return "0.0";
    case field_kind::boolean:
        return "false";
    default:
        return "{}";
    }
}

// A bridgeable field: its member name on each side plus the shared kind.
struct field_map {
    std::string row_member; // sql_column.name
    std::string dto_member; // property_member_identifier(prop.name)
    field_kind kind;
};

// A DTO candidate = an OpenAPI object schema whose fields are all required scalars/strings, keyed by
// normalized field name.
struct dto_candidate {
    std::string type_name;
    std::unordered_map<std::string, std::pair<std::string, field_kind>> fields; // norm → (member,kind)
};

} // namespace

std::string generate_bridge(const sql_catalog& catalog, const katana::openapi::document& doc,
                            const std::string& ns) {
    const std::string bridge_ns = ns.empty() ? "generated" : ns;

    // Collect eligible DTO candidates (plain object schemas: every property required scalar/string).
    std::vector<dto_candidate> candidates;
    for (const auto& s : doc.schemas) {
        if (s.kind != schema_kind::object || s.properties.empty()) {
            continue;
        }
        dto_candidate cand;
        cand.type_name = schema_identifier(doc, &s);
        bool eligible = true;
        for (const auto& prop : s.properties) {
            const field_kind k = dto_property_kind(prop.type);
            if (k == field_kind::other || is_optional_property(prop)) {
                eligible = false; // only bridge plain, required scalar/string DTOs
                break;
            }
            cand.fields.emplace(normalize(prop.name),
                                std::make_pair(property_member_identifier(prop.name), k));
        }
        if (eligible && !cand.fields.empty()) {
            candidates.push_back(std::move(cand));
        }
    }

    std::ostringstream out;
    out << "#pragma once\n\n";
    out << "// Auto-generated Row->DTO converters. Do not edit by hand.\n\n";
    out << "#include \"generated_dtos.hpp\"\n";
    out << "#include \"generated_sql_models.hpp\"\n";
    out << "#include \"katana/core/arena.hpp\"\n\n";
    out << "#include <optional>\n";
    out << "#include <string>\n";
    out << "#include <vector>\n\n";
    out << "namespace " << bridge_ns << " {\n\n";

    size_t emitted = 0;
    for (const auto& query : catalog.queries) {
        if (query.columns.empty()) {
            continue;
        }
        const std::string row_name = row_type_name(query);

        // Build the row's normalized field set.
        std::unordered_map<std::string, field_map> row_fields;
        bool row_ok = true;
        for (const auto& col : query.columns) {
            const field_kind k = row_column_kind(col.cpp_type);
            if (k == field_kind::other) {
                row_ok = false; // e.g. an array column — can't bridge this row
                break;
            }
            row_fields.emplace(normalize(col.name), field_map{col.name, "", k});
        }
        if (!row_ok) {
            continue;
        }

        // Find DTO candidates whose field set exactly matches (same keys + compatible kinds).
        std::vector<const dto_candidate*> matches;
        for (const auto& cand : candidates) {
            if (cand.fields.size() != row_fields.size()) {
                continue;
            }
            bool same = true;
            for (const auto& [norm, rf] : row_fields) {
                auto it = cand.fields.find(norm);
                if (it == cand.fields.end() || it->second.second != rf.kind) {
                    same = false;
                    break;
                }
            }
            if (same) {
                matches.push_back(&cand);
            }
        }

        if (matches.size() != 1) {
            if (matches.size() > 1) {
                std::cerr << "[bridge] " << row_name << ": ambiguous match ("
                          << matches.size() << " DTOs share its shape) — skipped\n";
            }
            continue; // no confident match
        }

        const dto_candidate& dto = *matches.front();
        // Resolve each row field's dto member name.
        std::vector<field_map> fields;
        for (auto& [norm, rf] : row_fields) {
            field_map fm = rf;
            fm.dto_member = dto.fields.at(norm).first;
            fields.push_back(fm);
        }

        // to_<Dto>(row, arena)
        out << "inline " << dto.type_name << " to_" << dto.type_name << "(const " << row_name
            << "& row, katana::monotonic_arena* arena) {\n";
        out << "    " << dto.type_name << " dto(arena);\n";
        for (const auto& fm : fields) {
            if (fm.kind == field_kind::string) {
                out << "    if (row." << fm.row_member << ") dto." << fm.dto_member << ".assign(row."
                    << fm.row_member << "->begin(), row." << fm.row_member << "->end());\n";
            } else if (fm.kind == field_kind::string_array) {
                out << "    if (row." << fm.row_member << ") {\n";
                out << "        for (const auto& elem : *row." << fm.row_member << ") {\n";
                out << "            katana::arena_string<> s(elem.begin(), elem.end(), "
                       "katana::arena_allocator<char>(arena));\n";
                out << "            dto." << fm.dto_member << ".push_back(std::move(s));\n";
                out << "        }\n";
                out << "    }\n";
            } else {
                out << "    dto." << fm.dto_member << " = row." << fm.row_member << ".value_or("
                    << zero_literal(fm.kind) << ");\n";
            }
        }
        out << "    return dto;\n}\n\n";
        ++emitted;
    }

    if (emitted == 0) {
        out << "// (no Row exactly matched a DTO — nothing to bridge)\n\n";
    }
    out << "} // namespace " << bridge_ns << "\n";
    return out.str();
}

} // namespace katana_gen

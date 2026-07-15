// TypeScript client generator: emits one self-contained `.ts` module from an OpenAPI document —
// interfaces/enums for the schemas and a typed `ApiClient` with one async method per operation.
// Mirrors the C++ generators' string-emitting shape (returns the file body; caller owns I/O), but
// produces TypeScript, so it deliberately does NOT use inject_namespace / with_layer.

#include "katana_gen/generator.hpp"

#include "katana/core/http.hpp"
#include "katana/core/openapi_ast.hpp"

#include <cctype>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace katana_gen {

using katana::openapi::document;
using katana::openapi::operation;
using katana::openapi::parameter;
using katana::openapi::schema;
using katana::openapi::schema_kind;

namespace {

// A TypeScript string-literal union for a string enum, e.g. `"active" | "archived"`.
std::string ts_enum_union(const schema& s) {
    std::string out;
    for (size_t i = 0; i < s.enum_values.size(); ++i) {
        if (i != 0) {
            out += " | ";
        }
        out += '"';
        out += std::string(s.enum_values[i].begin(), s.enum_values[i].end());
        out += '"';
    }
    return out.empty() ? "string" : out;
}

// The reference form of a schema's TypeScript type (a named type when the schema is a named
// object/enum, otherwise a structural type). Nullable schemas gain `| null`.
std::string ts_type(const document& doc, const schema* s) {
    if (s == nullptr) {
        return "unknown";
    }
    // A named string-enum is referenced by its type name; an anonymous one is inlined.
    if (s->kind == schema_kind::string && !s->enum_values.empty()) {
        std::string base = s->name.empty() ? ts_enum_union(*s) : schema_identifier(doc, s);
        return s->nullable ? base + " | null" : base;
    }

    std::string base;
    switch (s->kind) {
    case schema_kind::string:
        base = "string";
        break;
    case schema_kind::integer:
    case schema_kind::number:
        base = "number";
        break;
    case schema_kind::boolean:
        base = "boolean";
        break;
    case schema_kind::array:
        base = ts_type(doc, s->items) + "[]";
        break;
    case schema_kind::object:
        if (s->properties.empty()) {
            base = is_free_form_object(s) ? "Record<string, unknown>" : "Record<string, never>";
        } else {
            base = schema_identifier(doc, s);
        }
        break;
    default:
        base = "unknown";
        break;
    }
    return s->nullable ? base + " | null" : base;
}

// A property/field name safe to emit bare in an interface; quoted only when it isn't a plain
// TS identifier.
std::string ts_field_name(std::string_view name) {
    bool simple = !name.empty();
    for (size_t i = 0; i < name.size() && simple; ++i) {
        const char c = name[i];
        const bool ok = std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_' || c == '$';
        const bool digit_lead = i == 0 && std::isdigit(static_cast<unsigned char>(c)) != 0;
        if (!ok || digit_lead) {
            simple = false;
        }
    }
    return simple ? std::string(name) : "\"" + std::string(name) + "\"";
}

// Lowercase HTTP method verb for `fetch({ method })`.
std::string http_verb(katana::http::method m) {
    std::string v(katana::http::method_to_string(m));
    for (auto& c : v) {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    return v;
}

// The 2xx response body type of an operation, or `void` when there is none.
std::string operation_return_type(const document& doc, const operation& op) {
    const schema* best = nullptr;
    for (const auto& resp : op.responses) {
        if (resp.status >= 200 && resp.status < 300 && !resp.content.empty()) {
            best = resp.content.front().type;
            break;
        }
    }
    if (best == nullptr) {
        return "void";
    }
    return ts_type(doc, best);
}

// Emit the named interfaces / type aliases / enum unions for every meaningful schema.
void emit_types(std::ostream& out, const document& doc) {
    std::set<std::string> emitted;
    for (const auto& s : doc.schemas) {
        const std::string id = schema_identifier(doc, &s);
        if (id.empty() || !emitted.insert(id).second) {
            continue;
        }
        // Enum union.
        if (s.kind == schema_kind::string && !s.enum_values.empty() && !s.name.empty()) {
            out << "export type " << id << " = " << ts_enum_union(s) << ";\n\n";
            continue;
        }
        // Object interface.
        if (s.kind == schema_kind::object && !s.properties.empty()) {
            if (!s.description.empty()) {
                out << "/** " << s.description << " */\n";
            }
            out << "export interface " << id << " {\n";
            for (const auto& prop : s.properties) {
                const bool optional = !prop.required;
                out << "  " << ts_field_name(prop.name) << (optional ? "?" : "") << ": "
                    << ts_type(doc, prop.type) << ";\n";
            }
            out << "}\n\n";
            continue;
        }
        // Everything else (scalar/array/free-form aliases, anonymous inline schemas) is inlined
        // structurally at each use site by ts_type(), so no named alias is emitted — this keeps the
        // client free of `Sku_t = string` / `schema_N = Record<…>` noise.
    }
}

// Emit one typed async method per operation on the ApiClient class.
void emit_method(std::ostream& out, const document& doc, std::string_view path, const operation& op) {
    std::string op_id = op.operation_id.empty()
                            ? sanitize_identifier(std::string(http_verb(op.method)) + "_" +
                                                  std::string(path))
                            : std::string(op.operation_id);

    // Split parameters into path (positional, required) and query (bundled into one object).
    std::vector<const parameter*> path_params;
    std::vector<const parameter*> query_params;
    for (const auto& p : op.parameters) {
        if (p.in == katana::openapi::param_location::path) {
            path_params.push_back(&p);
        } else if (p.in == katana::openapi::param_location::query) {
            query_params.push_back(&p);
        }
    }

    const bool has_body = op.body != nullptr && !op.body->content.empty();
    const std::string body_type = has_body ? ts_type(doc, op.body->content.front().type) : "";
    const bool query_required = [&] {
        for (const auto* p : query_params) {
            if (p->required) {
                return true;
            }
        }
        return false;
    }();

    // Signature: required path params → body → query object → opts (optional). Keeping required
    // parameters first satisfies TS's no-optional-before-required rule.
    std::ostringstream sig;
    bool first = true;
    auto comma = [&] {
        if (!first) {
            sig << ", ";
        }
        first = false;
    };
    for (const auto* p : path_params) {
        comma();
        sig << sanitize_identifier(p->name) << ": " << ts_type(doc, p->type);
    }
    if (has_body) {
        comma();
        sig << "body: " << body_type;
    }
    if (!query_params.empty()) {
        comma();
        sig << "query" << (query_required ? "" : "?") << ": { ";
        for (size_t i = 0; i < query_params.size(); ++i) {
            const auto* p = query_params[i];
            if (i != 0) {
                sig << "; ";
            }
            sig << ts_field_name(p->name) << (p->required ? "" : "?") << ": " << ts_type(doc, p->type);
        }
        sig << " }";
    }
    comma();
    sig << "opts?: RequestOptions";

    // Path template with {param} → ${encodeURIComponent(String(param))}.
    std::string tmpl(path);
    for (const auto* p : path_params) {
        const std::string token = "{" + std::string(p->name) + "}";
        const std::string repl = "${encodeURIComponent(String(" + sanitize_identifier(p->name) + "))}";
        for (size_t pos = tmpl.find(token); pos != std::string::npos; pos = tmpl.find(token)) {
            tmpl.replace(pos, token.size(), repl);
        }
    }

    const std::string ret = operation_return_type(doc, op);
    if (!op.summary.empty()) {
        out << "  /** " << std::string(op.summary.begin(), op.summary.end()) << " */\n";
    }
    out << "  " << sanitize_identifier(op_id) << "(" << sig.str() << "): Promise<" << ret << "> {\n";
    out << "    return this.request<" << ret << ">(\"" << http_verb(op.method) << "\", `" << tmpl
        << "`, " << (query_params.empty() ? "undefined" : "query") << ", "
        << (has_body ? "body" : "undefined") << ", opts);\n";
    out << "  }\n\n";
}

} // namespace

std::string generate_typescript_client(const document& doc, const std::string& /*ns*/) {
    std::ostringstream out;
    out << "// Auto-generated TypeScript client from OpenAPI specification. Do not edit by hand.\n";
    out << "//\n// Regenerate with: katana_gen openapi -i <spec> --emit typescript -o <dir>\n\n";
    out << "/* eslint-disable */\n\n";

    emit_types(out, doc);

    out << "// ============================================================\n";
    out << "// Client\n";
    out << "// ============================================================\n\n";
    out << "export interface RequestOptions {\n";
    out << "  signal?: AbortSignal;\n";
    out << "  headers?: Record<string, string>;\n";
    out << "}\n\n";
    out << "export interface ApiClientOptions {\n";
    out << "  /** Base URL of the API, e.g. \"http://localhost:8080\". */\n";
    out << "  baseUrl?: string;\n";
    out << "  /** Custom fetch implementation (defaults to the global fetch). */\n";
    out << "  fetch?: typeof fetch;\n";
    out << "  /** Static headers or a provider called per request (e.g. for auth tokens). */\n";
    out << "  headers?: Record<string, string> | (() => Record<string, string>);\n";
    out << "}\n\n";
    out << "export class ApiError extends Error {\n";
    out << "  constructor(public readonly status: number, public readonly body: string) {\n";
    out << "    super(`HTTP ${status}`);\n";
    out << "    this.name = \"ApiError\";\n";
    out << "  }\n";
    out << "}\n\n";

    out << "export class ApiClient {\n";
    out << "  private readonly baseUrl: string;\n";
    out << "  private readonly fetchFn: typeof fetch;\n";
    out << "  private readonly headers: Record<string, string> | (() => Record<string, string>);\n\n";
    out << "  constructor(options: ApiClientOptions = {}) {\n";
    out << "    this.baseUrl = (options.baseUrl ?? \"\").replace(/\\/$/, \"\");\n";
    // Bind to globalThis: a bare `fetch` called as `this.fetchFn(...)` would rebind `this` to the
    // client and throw \"Illegal invocation\" in the browser.
    out << "    this.fetchFn = options.fetch ?? fetch.bind(globalThis);\n";
    out << "    this.headers = options.headers ?? {};\n";
    out << "  }\n\n";

    out << "  private async request<T>(\n";
    out << "    method: string,\n";
    out << "    path: string,\n";
    out << "    query: Record<string, unknown> | undefined,\n";
    out << "    body: unknown,\n";
    out << "    opts: RequestOptions | undefined,\n";
    out << "  ): Promise<T> {\n";
    out << "    let url = this.baseUrl + path;\n";
    out << "    if (query) {\n";
    out << "      const qs = new URLSearchParams();\n";
    out << "      for (const [k, v] of Object.entries(query)) {\n";
    out << "        if (v !== undefined && v !== null) qs.append(k, String(v));\n";
    out << "      }\n";
    out << "      const s = qs.toString();\n";
    out << "      if (s) url += \"?\" + s;\n";
    out << "    }\n";
    out << "    const base = typeof this.headers === \"function\" ? this.headers() : this.headers;\n";
    out << "    const headers: Record<string, string> = { ...base, ...(opts?.headers ?? {}) };\n";
    out << "    let payload: string | undefined;\n";
    out << "    if (body !== undefined) {\n";
    out << "      headers[\"Content-Type\"] = \"application/json\";\n";
    out << "      payload = JSON.stringify(body);\n";
    out << "    }\n";
    out << "    const res = await this.fetchFn(url, { method, headers, body: payload, signal: opts?.signal });\n";
    out << "    if (!res.ok) throw new ApiError(res.status, await res.text().catch(() => \"\"));\n";
    out << "    if (res.status === 204) return undefined as T;\n";
    out << "    const text = await res.text();\n";
    out << "    return (text ? JSON.parse(text) : undefined) as T;\n";
    out << "  }\n\n";

    for (const auto& path : doc.paths) {
        for (const auto& op : path.operations) {
            emit_method(out, doc, std::string(path.path.begin(), path.path.end()), op);
        }
    }

    out << "}\n";
    return out.str();
}

} // namespace katana_gen

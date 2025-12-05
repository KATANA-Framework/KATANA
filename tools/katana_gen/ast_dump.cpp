#include "generator.hpp"

#include <sstream>
#include <string>

namespace katana_gen {

std::string dump_ast_summary(const document& doc) {
    std::ostringstream os;
    os << "{";
    os << "\"openapi\":\"" << escape_json(doc.openapi_version) << "\",";
    os << "\"title\":\"" << escape_json(doc.info_title) << "\",";
    os << "\"version\":\"" << escape_json(doc.info_version) << "\",";
    os << "\"paths\":[";
    bool first_path = true;
    for (const auto& p : doc.paths) {
        if (!first_path) {
            os << ",";
        }
        first_path = false;
        os << "{";
        os << "\"path\":\"" << escape_json(p.path) << "\",";
        os << "\"operations\":[";
        bool first_op = true;
        for (const auto& op : p.operations) {
            if (!first_op) {
                os << ",";
            }
            first_op = false;
            os << "{";
            os << "\"method\":\"" << escape_json(katana::http::method_to_string(op.method))
               << "\",";
            os << "\"operationId\":\"" << escape_json(op.operation_id) << "\",";
            os << "\"summary\":\"" << escape_json(op.summary) << "\",";

            os << "\"parameters\":[";
            bool first_param = true;
            for (const auto& param : op.parameters) {
                if (!first_param) {
                    os << ",";
                }
                first_param = false;
                os << "{";
                os << "\"name\":\"" << escape_json(param.name) << "\",";
                os << "\"in\":\"";
                switch (param.in) {
                case katana::openapi::param_location::path:
                    os << "path";
                    break;
                case katana::openapi::param_location::query:
                    os << "query";
                    break;
                case katana::openapi::param_location::header:
                    os << "header";
                    break;
                case katana::openapi::param_location::cookie:
                    os << "cookie";
                    break;
                }
                os << "\",";
                os << "\"required\":" << (param.required ? "true" : "false");
                os << "}";
            }
            os << "],";

            os << "\"requestBody\":";
            if (op.body && !op.body->content.empty()) {
                os << "{";
                os << "\"description\":\"" << escape_json(op.body->description) << "\",";
                os << "\"content\":[";
                bool first_media = true;
                for (const auto& media : op.body->content) {
                    if (!first_media) {
                        os << ",";
                    }
                    first_media = false;
                    os << "{";
                    os << "\"contentType\":\"" << escape_json(media.content_type) << "\"";
                    os << "}";
                }
                os << "]";
                os << "}";
            } else {
                os << "null";
            }
            os << ",";

            os << "\"responses\":[";
            bool first_resp = true;
            for (const auto& resp : op.responses) {
                if (!first_resp) {
                    os << ",";
                }
                first_resp = false;
                os << "{";
                os << "\"status\":" << resp.status << ",";
                os << "\"default\":" << (resp.is_default ? "true" : "false") << ",";
                os << "\"description\":\"" << escape_json(resp.description) << "\",";
                os << "\"content\":[";
                bool first_c = true;
                for (const auto& media : resp.content) {
                    if (!first_c) {
                        os << ",";
                    }
                    first_c = false;
                    os << "{";
                    os << "\"contentType\":\"" << escape_json(media.content_type) << "\"";
                    os << "}";
                }
                os << "]";
                os << "}";
            }
            os << "]";

            os << "}";
        }
        os << "]";
        os << "}";
    }
    os << "]";
    os << ",\"schemas\":[";
    bool first_schema = true;
    auto kind_name = [](katana::openapi::schema_kind k) {
        using katana::openapi::schema_kind;
        switch (k) {
        case schema_kind::object:
            return "object";
        case schema_kind::array:
            return "array";
        case schema_kind::string:
            return "string";
        case schema_kind::integer:
            return "integer";
        case schema_kind::number:
            return "number";
        case schema_kind::boolean:
            return "boolean";
        case schema_kind::null_type:
            return "null";
        default:
            return "unknown";
        }
    };
    for (const auto& s : doc.schemas) {
        if (!first_schema) {
            os << ",";
        }
        first_schema = false;
        os << "{";
        os << "\"id\":\"" << escape_json(schema_identifier(doc, &s)) << "\",";
        os << "\"name\":\"" << escape_json(s.name) << "\",";
        os << "\"kind\":\"" << kind_name(s.kind) << "\",";
        os << "\"properties\":[";
        bool first_prop = true;
        for (const auto& prop : s.properties) {
            if (!first_prop) {
                os << ",";
            }
            first_prop = false;
            os << "{";
            os << "\"name\":\"" << escape_json(prop.name) << "\",";
            os << "\"required\":" << (prop.required ? "true" : "false") << ",";
            os << "\"kind\":\"" << (prop.type ? kind_name(prop.type->kind) : "unknown") << "\"";
            os << "}";
        }
        os << "]";
        os << "}";
    }
    os << "]";
    os << "}";
    return os.str();
}

} // namespace katana_gen

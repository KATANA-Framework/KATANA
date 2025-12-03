#include "katana/core/arena.hpp"
#include "katana/core/openapi_loader.hpp"

#include <cstddef>
#include <cstdint>
#include <span>

extern "C" int32_t LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size == 0 || size > 1024 * 1024) { // Max 1MB
        return 0;
    }

    katana::monotonic_arena arena;

    try {
        auto doc = katana::openapi::load(
            std::string_view(reinterpret_cast<const char*>(data), size), arena);

        if (doc) {
            // Access document fields to ensure no crashes
            [[maybe_unused]] const auto& version = doc->openapi_version;
            [[maybe_unused]] const auto& title = doc->info_title;
            [[maybe_unused]] const auto& info_ver = doc->info_version;
            [[maybe_unused]] auto schema_count = doc->schemas.size();
            [[maybe_unused]] auto path_count = doc->paths.size();

            // Iterate through schemas
            for (const auto& schema : doc->schemas) {
                [[maybe_unused]] const auto& name = schema.name;
                [[maybe_unused]] auto kind = schema.kind;
                [[maybe_unused]] auto prop_count = schema.properties.size();
            }

            // Iterate through paths
            for (const auto& path : doc->paths) {
                [[maybe_unused]] const auto& path_str = path.path;
                for (const auto& op : path.operations) {
                    [[maybe_unused]] auto method = op.method;
                    [[maybe_unused]] const auto& op_id = op.operation_id;
                    [[maybe_unused]] auto param_count = op.parameters.size();
                    [[maybe_unused]] auto resp_count = op.responses.size();
                }
            }
        }
    } catch (...) {
        // Fuzzer should not crash, but may throw expected exceptions
    }

    return 0;
}

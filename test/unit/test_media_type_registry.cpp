#include "katana/core/media_type_registry.hpp"

#include <gtest/gtest.h>

#include <array>

using namespace katana::http;

TEST(MediaTypeRegistry, DefaultInstanceContainsStage3Formats) {
    const auto& registry = media_type_registry::default_instance();

    ASSERT_NE(registry.find("application/json"), nullptr);
    EXPECT_EQ(registry.find("application/json")->format, media_format::json);
    ASSERT_NE(registry.find("application/cbor"), nullptr);
    EXPECT_EQ(registry.find("application/cbor")->format, media_format::cbor);
    ASSERT_NE(registry.find("application/msgpack"), nullptr);
    EXPECT_EQ(registry.find("application/msgpack")->format, media_format::msgpack);
    ASSERT_NE(registry.find("application/x-msgpack"), nullptr);
    EXPECT_EQ(registry.find("application/x-msgpack")->format, media_format::msgpack);
}

TEST(MediaTypeRegistry, RegisterTypeRejectsDuplicatesAndCapsStorage) {
    media_type_registry registry;

    EXPECT_TRUE(registry.register_type({"application/json", media_format::json}));
    EXPECT_FALSE(registry.register_type({"application/json", media_format::json}));

    for (size_t i = 1; i < media_type_registry::max_entries; ++i) {
        std::string mime = "application/test-" + std::to_string(i);
        EXPECT_TRUE(registry.register_type({mime, media_format::unknown}));
    }

    EXPECT_FALSE(registry.register_type({"application/overflow", media_format::unknown}));
}

TEST(MediaTypeRegistry, NegotiatePrefersHigherQValue) {
    const auto& registry = media_type_registry::default_instance();

    const auto* negotiated = registry.negotiate("application/json;q=0.4, application/cbor;q=0.9");

    ASSERT_NE(negotiated, nullptr);
    EXPECT_EQ(negotiated->mime_type, "application/cbor");
    EXPECT_EQ(negotiated->format, media_format::cbor);
}

TEST(MediaTypeRegistry, NegotiateSupportsWildcards) {
    const auto& registry = media_type_registry::default_instance();

    const auto* negotiated = registry.negotiate("application/*;q=0.8, text/plain;q=0.9");

    ASSERT_NE(negotiated, nullptr);
    EXPECT_EQ(negotiated->mime_type, "application/json");
}

TEST(MediaTypeRegistry, InferMediaFormatHandlesKnownAndUnknownTypes) {
    EXPECT_EQ(infer_media_format("application/json; charset=utf-8"), media_format::json);
    EXPECT_EQ(infer_media_format("application/cbor"), media_format::cbor);
    EXPECT_EQ(infer_media_format("application/x-msgpack"), media_format::msgpack);
    EXPECT_EQ(infer_media_format("text/plain"), media_format::unknown);
}

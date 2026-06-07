// Unit tests for katana::tracing — W3C Trace Context parse/format and server-span derivation.

#include "katana/core/tracing.hpp"

#include <gtest/gtest.h>

namespace {

using namespace katana::tracing;

TEST(Tracing, ParsesValidTraceparent) {
    auto p = parse_traceparent("00-4bf92f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-01");
    ASSERT_TRUE(p.has_value());
    EXPECT_EQ(p->trace_id_hi, 0x4bf92f3577b34da6ULL);
    EXPECT_EQ(p->trace_id_lo, 0xa3ce929d0e0e4736ULL);
    EXPECT_EQ(p->parent_span_id, 0x00f067aa0ba902b7ULL);
    EXPECT_TRUE(p->sampled);
}

TEST(Tracing, RejectsMalformedOrZeroTraceparent) {
    EXPECT_FALSE(parse_traceparent("").has_value());
    EXPECT_FALSE(parse_traceparent("00-abc").has_value());                        // too short
    EXPECT_FALSE(parse_traceparent("00-4bf9_f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-01")
                     .has_value());                                                // bad hex
    EXPECT_FALSE(parse_traceparent("00-00000000000000000000000000000000-00f067aa0ba902b7-01")
                     .has_value());                                                // zero trace
    EXPECT_FALSE(parse_traceparent("00-4bf92f3577b34da6a3ce929d0e0e4736-0000000000000000-01")
                     .has_value());                                                // zero span
    EXPECT_FALSE(parse_traceparent("00X4bf92f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-01")
                     .has_value());                                                // bad delimiter
}

TEST(Tracing, UnsampledFlag) {
    auto p = parse_traceparent("00-4bf92f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-00");
    ASSERT_TRUE(p.has_value());
    EXPECT_FALSE(p->sampled);
}

TEST(Tracing, ServerSpanContinuesInboundTrace) {
    auto sc = start_server_span("00-4bf92f3577b34da6a3ce929d0e0e4736-00f067aa0ba902b7-01");
    EXPECT_TRUE(sc.valid());
    EXPECT_EQ(sc.trace_id_hex(), "4bf92f3577b34da6a3ce929d0e0e4736");
    EXPECT_EQ(sc.parent_span_id_hex(), "00f067aa0ba902b7");
    EXPECT_NE(sc.span_id_hex(), "00f067aa0ba902b7"); // fresh span for this hop
    EXPECT_EQ(sc.span_id_hex().size(), 16u);
    EXPECT_TRUE(sc.sampled);
    // to_traceparent re-encodes THIS span for downstream propagation.
    const std::string tp = sc.to_traceparent();
    EXPECT_EQ(tp.substr(0, 3), "00-");
    EXPECT_EQ(tp.substr(3, 32), "4bf92f3577b34da6a3ce929d0e0e4736");
    EXPECT_EQ(tp.substr(36, 16), sc.span_id_hex());
    EXPECT_EQ(tp.substr(52), "-01");
}

TEST(Tracing, ServerSpanStartsNewRootWhenNoInbound) {
    auto a = start_server_span(std::nullopt);
    auto b = start_server_span(std::nullopt);
    EXPECT_TRUE(a.valid());
    EXPECT_EQ(a.parent_span_id, 0u); // root has no parent
    EXPECT_TRUE(a.sampled);          // always-on default sampler
    // Distinct trace ids across roots.
    EXPECT_FALSE(a.trace_id_hi == b.trace_id_hi && a.trace_id_lo == b.trace_id_lo);
}

TEST(Tracing, ServerSpanStartsNewRootWhenInboundMalformed) {
    auto sc = start_server_span("garbage-not-a-traceparent");
    EXPECT_TRUE(sc.valid());
    EXPECT_EQ(sc.parent_span_id, 0u);
}

} // namespace

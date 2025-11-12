#include "katana/core/http_field.hpp"
#include <algorithm>
#include <cctype>

namespace katana::http {

namespace detail {

constexpr std::array<std::string_view, static_cast<size_t>(field::MAX_FIELD_VALUE)> field_name_table = {{
    "unknown",
    "A-IM",
    "Accept",
    "Accept-Additions",
    "Accept-Charset",
    "Accept-Datetime",
    "Accept-Encoding",
    "Accept-Features",
    "Accept-Language",
    "Accept-Patch",
    "Accept-Post",
    "Accept-Ranges",
    "Access-Control",
    "Access-Control-Allow-Credentials",
    "Access-Control-Allow-Headers",
    "Access-Control-Allow-Methods",
    "Access-Control-Allow-Origin",
    "Access-Control-Expose-Headers",
    "Access-Control-Max-Age",
    "Access-Control-Request-Headers",
    "Access-Control-Request-Method",
    "Age",
    "Allow",
    "ALPN",
    "Also-Control",
    "Alt-Svc",
    "Alt-Used",
    "Alternate-Recipient",
    "Alternates",
    "Apparently-To",
    "Apply-To-Redirect-Ref",
    "Approved",
    "Archive",
    "Archived-At",
    "Article-Names",
    "Article-Updates",
    "Authentication-Control",
    "Authentication-Info",
    "Authentication-Results",
    "Authorization",
    "Auto-Submitted",
    "Autoforwarded",
    "Autosubmitted",
    "Base",
    "Bcc",
    "Body",
    "C-Ext",
    "C-Man",
    "C-Opt",
    "C-PEP",
    "C-PEP-Info",
    "Cache-Control",
    "CalDAV-Timezones",
    "Cancel-Key",
    "Cancel-Lock",
    "Cc",
    "Close",
    "Comments",
    "Compliance",
    "Connection",
    "Content-Alternative",
    "Content-Base",
    "Content-Description",
    "Content-Disposition",
    "Content-Duration",
    "Content-Encoding",
    "Content-Features",
    "Content-ID",
    "Content-Identifier",
    "Content-Language",
    "Content-Length",
    "Content-Location",
    "Content-MD5",
    "Content-Range",
    "Content-Return",
    "Content-Script-Type",
    "Content-Style-Type",
    "Content-Transfer-Encoding",
    "Content-Type",
    "Content-Version",
    "Control",
    "Conversion",
    "Conversion-With-Loss",
    "Cookie",
    "Cookie2",
    "Cost",
    "DASL",
    "Date",
    "Date-Received",
    "DAV",
    "Default-Style",
    "Deferred-Delivery",
    "Delivery-Date",
    "Delta-Base",
    "Depth",
    "Derived-From",
    "Destination",
    "Differential-ID",
    "Digest",
    "Discarded-X400-IPMS-Extensions",
    "Discarded-X400-MTS-Extensions",
    "Disclose-Recipients",
    "Disposition-Notification-Options",
    "Disposition-Notification-To",
    "Distribution",
    "DKIM-Signature",
    "DL-Expansion-History",
    "Downgraded-Bcc",
    "Downgraded-Cc",
    "Downgraded-Disposition-Notification-To",
    "Downgraded-Final-Recipient",
    "Downgraded-From",
    "Downgraded-In-Reply-To",
    "Downgraded-Mail-From",
    "Downgraded-Message-Id",
    "Downgraded-Original-Recipient",
    "Downgraded-Rcpt-To",
    "Downgraded-References",
    "Downgraded-Reply-To",
    "Downgraded-Resent-Bcc",
    "Downgraded-Resent-Cc",
    "Downgraded-Resent-From",
    "Downgraded-Resent-Reply-To",
    "Downgraded-Resent-Sender",
    "Downgraded-Resent-To",
    "Downgraded-Return-Path",
    "Downgraded-Sender",
    "Downgraded-To",
    "EDIINT-Features",
    "Eesst-Version",
    "Encoding",
    "Encrypted",
    "Errors-To",
    "ETag",
    "Expect",
    "Expires",
    "Expiry-Date",
    "Ext",
    "Followup-To",
    "Forwarded",
    "From",
    "Generate-Delivery-Report",
    "GetProfile",
    "Hobareg",
    "Host",
    "HTTP2-Settings",
    "If",
    "If-Match",
    "If-Modified-Since",
    "If-None-Match",
    "If-Range",
    "If-Schedule-Tag-Match",
    "If-Unmodified-Since",
    "IM",
    "Importance",
    "In-Reply-To",
    "Incomplete-Copy",
    "Injection-Date",
    "Injection-Info",
    "Jabber-ID",
    "Keep-Alive",
    "Keywords",
    "Label",
    "Language",
    "Last-Modified",
    "Latest-Delivery-Time",
    "Lines",
    "Link",
    "List-Archive",
    "List-Help",
    "List-ID",
    "List-Owner",
    "List-Post",
    "List-Subscribe",
    "List-Unsubscribe",
    "List-Unsubscribe-Post",
    "Location",
    "Lock-Token",
    "Man",
    "Max-Forwards",
    "Memento-Datetime",
    "Message-Context",
    "Message-ID",
    "Message-Type",
    "Meter",
    "Method-Check",
    "Method-Check-Expires",
    "MIME-Version",
    "MMHS-Acp127-Message-Identifier",
    "MMHS-Authorizing-Users",
    "MMHS-Codress-Message-Indicator",
    "MMHS-Copy-Precedence",
    "MMHS-Exempted-Address",
    "MMHS-Extended-Authorisation-Info",
    "MMHS-Handling-Instructions",
    "MMHS-Message-Instructions",
    "MMHS-Message-Type",
    "MMHS-Originator-PLAD",
    "MMHS-Originator-Reference",
    "MMHS-Other-Recipients-Indicator-CC",
    "MMHS-Other-Recipients-Indicator-To",
    "MMHS-Primary-Precedence",
    "MMHS-Subject-Indicator-Codes",
    "MT-Priority",
    "Negotiate",
    "Newsgroups",
    "NNTP-Posting-Date",
    "NNTP-Posting-Host",
    "Non-Compliance",
    "Obsoletes",
    "Opt",
    "Optional",
    "Optional-WWW-Authenticate",
    "Ordering-Type",
    "Organization",
    "Origin",
    "Original-Encoded-Information-Types",
    "Original-From",
    "Original-Message-ID",
    "Original-Recipient",
    "Original-Sender",
    "Original-Subject",
    "Originator-Return-Address",
    "Overwrite",
    "P3P",
    "Path",
    "PEP",
    "Pep-Info",
    "PICS-Label",
    "Position",
    "Posting-Version",
    "Pragma",
    "Prefer",
    "Preference-Applied",
    "Prevent-NonDelivery-Report",
    "Priority",
    "Privicon",
    "ProfileObject",
    "Protocol",
    "Protocol-Info",
    "Protocol-Query",
    "Protocol-Request",
    "Proxy-Authenticate",
    "Proxy-Authentication-Info",
    "Proxy-Authorization",
    "Proxy-Connection",
    "Proxy-Features",
    "Proxy-Instruction",
    "Public",
    "Public-Key-Pins",
    "Public-Key-Pins-Report-Only",
    "Range",
    "Received",
    "Received-SPF",
    "Redirect-Ref",
    "References",
    "Referer",
    "Referer-Root",
    "Relay-Version",
    "Reply-By",
    "Reply-To",
    "Require-Recipient-Valid-Since",
    "Resent-Bcc",
    "Resent-Cc",
    "Resent-Date",
    "Resent-From",
    "Resent-Message-ID",
    "Resent-Reply-To",
    "Resent-Sender",
    "Resent-To",
    "Resolution-Hint",
    "Resolver-Location",
    "Retry-After",
    "Return-Path",
    "Safe",
    "Schedule-Reply",
    "Schedule-Tag",
    "Sec-Fetch-Dest",
    "Sec-Fetch-Mode",
    "Sec-Fetch-Site",
    "Sec-Fetch-User",
    "Sec-WebSocket-Accept",
    "Sec-WebSocket-Extensions",
    "Sec-WebSocket-Key",
    "Sec-WebSocket-Protocol",
    "Sec-WebSocket-Version",
    "Security-Scheme",
    "See-Also",
    "Sender",
    "Sensitivity",
    "Server",
    "Set-Cookie",
    "Set-Cookie2",
    "SetProfile",
    "SIO-Label",
    "SIO-Label-History",
    "Slug",
    "SoapAction",
    "Solicitation",
    "Status-URI",
    "Strict-Transport-Security",
    "Subject",
    "SubOK",
    "Subst",
    "Summary",
    "Supersedes",
    "Surrogate-Capability",
    "Surrogate-Control",
    "TCN",
    "TE",
    "Timeout",
    "Title",
    "To",
    "Topic",
    "Trailer",
    "Transfer-Encoding",
    "TTL",
    "UA-Color",
    "UA-Media",
    "UA-Pixels",
    "UA-Resolution",
    "UA-Windowpixels",
    "Upgrade",
    "Urgency",
    "URI",
    "User-Agent",
    "Variant-Vary",
    "Vary",
    "VBR-Info",
    "Version",
    "Via",
    "Want-Digest",
    "Warning",
    "WWW-Authenticate",
    "X-Archived-At",
    "X-Device-Accept",
    "X-Device-Accept-Charset",
    "X-Device-Accept-Encoding",
    "X-Device-Accept-Language",
    "X-Device-User-Agent",
    "X-Frame-Options",
    "X-Mittente",
    "X-PGP-Sig",
    "X-Ricevuta",
    "X-Riferimento-Message-ID",
    "X-TipoRicevuta",
    "X-Trasporto",
    "X-VerificaSicurezza",
    "X400-Content-Identifier",
    "X400-Content-Return",
    "X400-Content-Type",
    "X400-MTS-Identifier",
    "X400-Originator",
    "X400-Received",
    "X400-Recipients",
    "X400-Trace",
    "Xref"
}};

inline bool case_insensitive_equal(std::string_view a, std::string_view b) noexcept {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        char ca = (a[i] >= 'A' && a[i] <= 'Z') ? (a[i] + 32) : a[i];
        char cb = (b[i] >= 'A' && b[i] <= 'Z') ? (b[i] + 32) : b[i];
        if (ca != cb) {
            return false;
        }
    }
    return true;
}

inline bool case_insensitive_less(std::string_view a, std::string_view b) noexcept {
    size_t min_size = std::min(a.size(), b.size());
    for (size_t i = 0; i < min_size; ++i) {
        char ca = (a[i] >= 'A' && a[i] <= 'Z') ? (a[i] + 32) : a[i];
        char cb = (b[i] >= 'A' && b[i] <= 'Z') ? (b[i] + 32) : b[i];
        if (ca < cb) return true;
        if (ca > cb) return false;
    }
    return a.size() < b.size();
}

// Top 25 most common HTTP headers (linear search, ~22 ns)
constexpr std::array<field_entry, 25> popular_headers = {{
    {field_name_table[154], field::host, fnv1a_hash(field_name_table[154])},
    {field_name_table[335], field::user_agent, fnv1a_hash(field_name_table[335])},
    {field_name_table[12], field::accept, fnv1a_hash(field_name_table[12])},
    {field_name_table[16], field::accept_encoding, fnv1a_hash(field_name_table[16])},
    {field_name_table[18], field::accept_language, fnv1a_hash(field_name_table[18])},
    {field_name_table[88], field::content_type, fnv1a_hash(field_name_table[88])},
    {field_name_table[80], field::content_length, fnv1a_hash(field_name_table[80])},
    {field_name_table[69], field::connection, fnv1a_hash(field_name_table[69])},
    {field_name_table[61], field::cache_control, fnv1a_hash(field_name_table[61])},
    {field_name_table[93], field::cookie, fnv1a_hash(field_name_table[93])},
    {field_name_table[49], field::authorization, fnv1a_hash(field_name_table[49])},
    {field_name_table[266], field::referer, fnv1a_hash(field_name_table[266])},
    {field_name_table[225], field::origin, fnv1a_hash(field_name_table[225])},
    {field_name_table[97], field::date, fnv1a_hash(field_name_table[97])},
    {field_name_table[300], field::server, fnv1a_hash(field_name_table[300])},
    {field_name_table[301], field::set_cookie, fnv1a_hash(field_name_table[301])},
    {field_name_table[325], field::transfer_encoding, fnv1a_hash(field_name_table[325])},
    {field_name_table[158], field::if_modified_since, fnv1a_hash(field_name_table[158])},
    {field_name_table[159], field::if_none_match, fnv1a_hash(field_name_table[159])},
    {field_name_table[143], field::etag, fnv1a_hash(field_name_table[143])},
    {field_name_table[145], field::expires, fnv1a_hash(field_name_table[145])},
    {field_name_table[174], field::last_modified, fnv1a_hash(field_name_table[174])},
    {field_name_table[337], field::vary, fnv1a_hash(field_name_table[337])},
    {field_name_table[26], field::access_control_allow_origin, fnv1a_hash(field_name_table[26])},
    {field_name_table[75], field::content_encoding, fnv1a_hash(field_name_table[75])}
}};

// Rare headers sorted alphabetically for binary search (log₂342 = 9 comparisons, ~64 ns)
constexpr std::array<field_entry, 342> create_rare_headers() {
    std::array<field_entry, 342> result{};
    size_t idx = 0;

    for (size_t i = 0; i < field_name_table.size(); ++i) {
        field fld = static_cast<field>(i);

        // Skip popular headers
        if (fld == field::host || fld == field::user_agent || fld == field::accept ||
            fld == field::accept_encoding || fld == field::accept_language ||
            fld == field::content_type || fld == field::content_length ||
            fld == field::connection || fld == field::cache_control ||
            fld == field::cookie || fld == field::authorization ||
            fld == field::referer || fld == field::origin || fld == field::date ||
            fld == field::server || fld == field::set_cookie ||
            fld == field::transfer_encoding || fld == field::if_modified_since ||
            fld == field::if_none_match || fld == field::etag || fld == field::expires ||
            fld == field::last_modified || fld == field::vary ||
            fld == field::access_control_allow_origin || fld == field::content_encoding) {
            continue;
        }

        result[idx++] = {field_name_table[i], fld, fnv1a_hash(field_name_table[i])};
    }

    // Bubble sort (constexpr compatible)
    for (size_t i = 0; i < result.size(); ++i) {
        for (size_t j = i + 1; j < result.size(); ++j) {
            if (case_insensitive_less(result[j].name, result[i].name)) {
                auto tmp = result[i];
                result[i] = result[j];
                result[j] = tmp;
            }
        }
    }

    return result;
}

const std::array<field_entry, 342> rare_headers = create_rare_headers();

}

field string_to_field(std::string_view name) noexcept {
    if (name.empty()) {
        return field::unknown;
    }

    uint32_t hash = detail::fnv1a_hash(name);

    // Fast path: linear search in popular headers (95%+ of requests)
    for (const auto& entry : detail::popular_headers) {
        if (entry.hash == hash && detail::case_insensitive_equal(entry.name, name)) {
            return entry.value;
        }
    }

    // Slow path: binary search in rare headers (5% of requests)
    auto it = std::lower_bound(
        detail::rare_headers.begin(),
        detail::rare_headers.end(),
        name,
        [](const detail::field_entry& entry, std::string_view n) {
            return detail::case_insensitive_less(entry.name, n);
        }
    );

    if (it != detail::rare_headers.end() && detail::case_insensitive_equal(it->name, name)) {
        return it->value;
    }

    return field::unknown;
}

std::string_view field_to_string(field f) noexcept {
    auto idx = static_cast<size_t>(f);
    if (idx < detail::field_name_table.size()) {
        return detail::field_name_table[idx];
    }
    return "unknown";
}

} // namespace katana::http

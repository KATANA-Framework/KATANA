-- Pulse demo schema: a product catalog and an analytics event store. Two logical domains,
-- served by one binary from two namespaced contracts (catalog + analytics).

CREATE TABLE IF NOT EXISTS pulse_products (
    id          BIGSERIAL PRIMARY KEY,
    sku         TEXT        NOT NULL UNIQUE,
    name        TEXT        NOT NULL,
    price_cents BIGINT      NOT NULL,
    tags        JSONB       NOT NULL DEFAULT '[]'::jsonb,
    created_at  TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE TABLE IF NOT EXISTS pulse_events (
    id         BIGSERIAL PRIMARY KEY,
    kind       TEXT        NOT NULL,         -- e.g. "view", "add_to_cart", "purchase"
    product_id BIGINT,                       -- optional reference to a product
    props      JSONB       NOT NULL DEFAULT '{}'::jsonb,
    created_at TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE INDEX IF NOT EXISTS pulse_events_kind_idx ON pulse_events (kind);

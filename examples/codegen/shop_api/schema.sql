DROP TABLE IF EXISTS shop_order_items, shop_orders, shop_products, shop_customers CASCADE;
CREATE TABLE shop_customers (
  id bigserial PRIMARY KEY, name text NOT NULL, email text NOT NULL UNIQUE,
  country text NOT NULL, created_at timestamptz NOT NULL DEFAULT now());
CREATE TABLE shop_products (
  id bigserial PRIMARY KEY, name text NOT NULL, category text NOT NULL,
  price float8 NOT NULL, stock bigint NOT NULL DEFAULT 0);
CREATE TABLE shop_orders (
  id bigserial PRIMARY KEY, customer_id bigint NOT NULL REFERENCES shop_customers(id),
  status text NOT NULL, created_at timestamptz NOT NULL DEFAULT now());
CREATE TABLE shop_order_items (
  id bigserial PRIMARY KEY, order_id bigint NOT NULL REFERENCES shop_orders(id),
  product_id bigint NOT NULL REFERENCES shop_products(id),
  qty bigint NOT NULL, unit_price float8 NOT NULL);
CREATE INDEX ix_orders_customer ON shop_orders(customer_id);
CREATE INDEX ix_items_order ON shop_order_items(order_id);
CREATE INDEX ix_items_product ON shop_order_items(product_id);
CREATE INDEX ix_customers_country ON shop_customers(country);

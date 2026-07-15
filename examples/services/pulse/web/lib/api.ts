// Thin facade over the *generated* TypeScript clients (katana_gen openapi --emit typescript). The
// types and request logic come straight from the OpenAPI contracts — this file only wires the two
// clients to one base URL and gives the UI short method names. Regenerate the clients with the
// build (katana_add_typescript) — do not hand-edit web/lib/catalog|analytics/generated_client.ts.

import {
  ApiClient as CatalogClient,
  type Product,
  type ProductList,
  type CreateProductRequest,
} from "./catalog/generated_client";
import {
  ApiClient as AnalyticsClient,
  type EventSummary,
  type EventCount,
  type IngestEventRequest,
  type IngestEventResponse,
} from "./analytics/generated_client";

const BASE = process.env.NEXT_PUBLIC_PULSE_URL ?? "http://localhost:8080";

const catalog = new CatalogClient({ baseUrl: BASE });
const analytics = new AnalyticsClient({ baseUrl: BASE });

export type {
  Product,
  ProductList,
  CreateProductRequest,
  EventSummary,
  EventCount,
  IngestEventRequest,
  IngestEventResponse,
};

export const api = {
  baseUrl: BASE,
  listProducts: (limit = 20) => catalog.list_products({ limit }),
  createProduct: (p: CreateProductRequest) => catalog.create_product(p),
  summary: () => analytics.event_summary(),
  ingest: (e: IngestEventRequest) => analytics.ingest_event(e),
};

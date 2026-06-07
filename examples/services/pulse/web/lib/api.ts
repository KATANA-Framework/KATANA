// Typed client for the Pulse API (catalog + analytics). CORS is enabled server-side, so these
// run from the browser directly against the Go-less C++ backend.

const BASE = process.env.NEXT_PUBLIC_PULSE_URL ?? "http://localhost:8080";

export type Product = {
  id: number;
  sku: string;
  name: string;
  price_cents: number;
  created_at: string;
};
export type ProductList = { products: Product[]; count: number };
export type CreateProduct = { sku: string; name: string; price_cents: number };

export type EventCount = { kind: string; count: number };
export type EventSummary = { kinds: EventCount[]; total: number };
export type IngestEvent = { kind: string; product_id?: number; props?: Record<string, unknown> };

async function req<T>(path: string, init?: RequestInit): Promise<T> {
  const res = await fetch(`${BASE}${path}`, {
    ...init,
    headers: { "Content-Type": "application/json", ...(init?.headers ?? {}) },
    cache: "no-store",
  });
  if (!res.ok) {
    let detail = "";
    try {
      detail = (await res.json())?.detail ?? "";
    } catch {
      /* ignore */
    }
    throw new Error(`${init?.method ?? "GET"} ${path} → ${res.status}${detail ? `: ${detail}` : ""}`);
  }
  if (res.status === 204) return undefined as T;
  return (await res.json()) as T;
}

export const api = {
  baseUrl: BASE,
  listProducts: (limit = 20) => req<ProductList>(`/products?limit=${limit}`),
  createProduct: (p: CreateProduct) =>
    req<Product>(`/products`, { method: "POST", body: JSON.stringify(p) }),
  summary: () => req<EventSummary>(`/analytics/summary`),
  ingest: (e: IngestEvent) =>
    req<{ id: number; kind: string }>(`/events`, { method: "POST", body: JSON.stringify(e) }),
};

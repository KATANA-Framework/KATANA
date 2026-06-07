"use client";

import { useCallback, useEffect, useState } from "react";
import { api, type Product, type EventSummary } from "@/lib/api";

export default function Dashboard() {
  const [products, setProducts] = useState<Product[]>([]);
  const [summary, setSummary] = useState<EventSummary | null>(null);
  const [err, setErr] = useState<string>("");

  const refresh = useCallback(async () => {
    try {
      const [list, sum] = await Promise.all([api.listProducts(50), api.summary()]);
      setProducts(list.products);
      setSummary(sum);
      setErr("");
    } catch (e) {
      setErr(String(e instanceof Error ? e.message : e));
    }
  }, []);

  useEffect(() => {
    refresh();
    const t = setInterval(refresh, 5000); // poll the cached endpoints
    return () => clearInterval(t);
  }, [refresh]);

  return (
    <div className="container">
      <header className="top">
        <h1>Pulse</h1>
        <span className="sub">
          catalog + analytics on KATANA · <span className="pill">{api.baseUrl}</span>
        </span>
      </header>
      <p className="muted" style={{ marginTop: 0 }}>
        A Next.js frontend talking cross-origin (CORS) to the C++ service. Reads are cached
        server-side (x-katana-cache); event writes are rate-limited + idempotent.
      </p>

      {err && <div className="err">⚠ {err} — is the Pulse server running?</div>}

      <div className="grid">
        <CatalogCard products={products} onChange={refresh} onError={setErr} />
        <AnalyticsCard summary={summary} onChange={refresh} onError={setErr} products={products} />
      </div>
    </div>
  );
}

function CatalogCard({
  products,
  onChange,
  onError,
}: {
  products: Product[];
  onChange: () => void;
  onError: (m: string) => void;
}) {
  const [sku, setSku] = useState("");
  const [name, setName] = useState("");
  const [price, setPrice] = useState("");
  const [busy, setBusy] = useState(false);

  const create = async () => {
    setBusy(true);
    try {
      await api.createProduct({ sku, name, price_cents: Math.round(Number(price) * 100) || 0 });
      setSku("");
      setName("");
      setPrice("");
      onChange();
    } catch (e) {
      onError(String(e instanceof Error ? e.message : e));
    } finally {
      setBusy(false);
    }
  };

  return (
    <section className="card">
      <h2>Catalog · {products.length} products</h2>
      <div className="row" style={{ marginBottom: 12 }}>
        <input placeholder="SKU" value={sku} onChange={(e) => setSku(e.target.value)} />
        <input placeholder="Name" value={name} onChange={(e) => setName(e.target.value)} />
        <input
          placeholder="Price"
          type="number"
          step="0.01"
          value={price}
          onChange={(e) => setPrice(e.target.value)}
          style={{ maxWidth: 90 }}
        />
        <button onClick={create} disabled={busy || !sku || !name}>
          Add
        </button>
      </div>
      <table>
        <thead>
          <tr>
            <th>#</th>
            <th>SKU</th>
            <th>Name</th>
            <th style={{ textAlign: "right" }}>Price</th>
          </tr>
        </thead>
        <tbody>
          {products.map((p) => (
            <tr key={p.id}>
              <td className="muted">{p.id}</td>
              <td>{p.sku}</td>
              <td>{p.name}</td>
              <td style={{ textAlign: "right" }}>${(p.price_cents / 100).toFixed(2)}</td>
            </tr>
          ))}
          {products.length === 0 && (
            <tr>
              <td colSpan={4} className="muted">
                No products yet — add one above.
              </td>
            </tr>
          )}
        </tbody>
      </table>
    </section>
  );
}

function AnalyticsCard({
  summary,
  products,
  onChange,
  onError,
}: {
  summary: EventSummary | null;
  products: Product[];
  onChange: () => void;
  onError: (m: string) => void;
}) {
  const [kind, setKind] = useState("view");
  const [busy, setBusy] = useState(false);
  const max = Math.max(1, ...(summary?.kinds.map((k) => k.count) ?? [1]));

  const send = async () => {
    setBusy(true);
    try {
      await api.ingest({
        kind,
        product_id: products[0]?.id,
        props: { source: "web" },
      });
      onChange();
    } catch (e) {
      onError(String(e instanceof Error ? e.message : e));
    } finally {
      setBusy(false);
    }
  };

  return (
    <section className="card">
      <h2>Analytics · {summary?.total ?? 0} events</h2>
      <div className="row" style={{ marginBottom: 14 }}>
        <input placeholder="event kind" value={kind} onChange={(e) => setKind(e.target.value)} />
        <button onClick={send} disabled={busy || !kind}>
          Ingest
        </button>
        {["view", "add_to_cart", "purchase"].map((k) => (
          <button key={k} className="ghost" onClick={() => setKind(k)}>
            {k}
          </button>
        ))}
      </div>
      <table>
        <tbody>
          {(summary?.kinds ?? []).map((k) => (
            <tr key={k.kind}>
              <td style={{ width: 120 }}>{k.kind}</td>
              <td>
                <div className="bar" style={{ width: `${(k.count / max) * 100}%` }} />
              </td>
              <td style={{ textAlign: "right", width: 60 }} className="muted">
                {k.count}
              </td>
            </tr>
          ))}
          {(summary?.kinds.length ?? 0) === 0 && (
            <tr>
              <td className="muted">No events yet — ingest one above.</td>
            </tr>
          )}
        </tbody>
      </table>
    </section>
  );
}

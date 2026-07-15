"use client";

import { useCallback, useEffect, useState } from "react";
import { api, type Product, type EventSummary } from "@/lib/api";

export default function Console() {
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

  const kinds = summary?.kinds ?? [];
  const top = kinds.reduce<{ kind: string; count: number } | null>(
    (best, k) => (best && best.count >= k.count ? best : k),
    null
  );

  return (
    <main className="console">
      <div className="statusbar">
        <div className="brand">
          Pulse<span className="tag">KATANA · C++</span>
        </div>
        <span className="spacer" />
        <span className={`live${err ? " down" : ""}`}>
          <span className="dot" />
          {err ? "offline" : "live"}
        </span>
        <span className="endpoint">{api.baseUrl}</span>
        <ThemeToggle />
      </div>

      <p className="intro">
        A Next.js console talking cross-origin to the C++ service. Reads are cached server-side
        (<code>x-katana-cache</code>); event writes are rate-limited and idempotent. Types and the
        request layer are generated from the OpenAPI contracts.
      </p>

      {err && (
        <div className="banner">
          <AlertIcon />
          {err} — is the Pulse server running?
        </div>
      )}

      <div className="kpis">
        <Kpi label="Products" value={products.length} />
        <Kpi label="Events" value={summary?.total ?? 0} accent />
        <Kpi
          label="Top event"
          value={top ? top.count : 0}
          hint={top ? top.kind : "no events yet"}
        />
        <Kpi label="Event kinds" value={kinds.length} hint="distinct" />
      </div>

      <div className="workspace">
        <CatalogPanel products={products} onChange={refresh} onError={setErr} />
        <AnalyticsPanel summary={summary} products={products} onChange={refresh} onError={setErr} />
      </div>

      <p className="footnote">
        Backend: <code>pulse</code> (KATANA) · two OpenAPI + two SQL contracts, generated. Health at{" "}
        <code>/healthz</code>, metrics at <code>/metrics</code>.
      </p>
    </main>
  );
}

function Kpi({
  label,
  value,
  hint,
  accent,
}: {
  label: string;
  value: number;
  hint?: string;
  accent?: boolean;
}) {
  return (
    <div className="kpi">
      <div className="label">{label}</div>
      <div className={`value${accent ? " accent" : ""}`}>{value.toLocaleString()}</div>
      {hint && <div className="hint">{hint}</div>}
    </div>
  );
}

function ThemeToggle() {
  const [dark, setDark] = useState(false);

  useEffect(() => {
    const attr = document.documentElement.getAttribute("data-theme");
    setDark(attr ? attr === "dark" : window.matchMedia("(prefers-color-scheme: dark)").matches);
  }, []);

  const toggle = () => {
    const next = dark ? "light" : "dark";
    document.documentElement.setAttribute("data-theme", next);
    try {
      localStorage.setItem("pulse-theme", next);
    } catch {
      /* ignore */
    }
    setDark(!dark);
  };

  return (
    <button className="icon-btn" onClick={toggle} aria-label="Toggle color theme" title="Toggle theme">
      {dark ? <SunIcon /> : <MoonIcon />}
    </button>
  );
}

function MoonIcon() {
  return (
    <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" aria-hidden="true">
      <path d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z" />
    </svg>
  );
}

function SunIcon() {
  return (
    <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" aria-hidden="true">
      <circle cx="12" cy="12" r="4" />
      <path d="M12 2v2M12 20v2M4.93 4.93l1.41 1.41M17.66 17.66l1.41 1.41M2 12h2M20 12h2M6.34 17.66l-1.41 1.41M19.07 4.93l-1.41 1.41" />
    </svg>
  );
}

function AlertIcon() {
  return (
    <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="2" strokeLinecap="round" strokeLinejoin="round" aria-hidden="true" style={{ flexShrink: 0 }}>
      <path d="M10.29 3.86 1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z" />
      <line x1="12" y1="9" x2="12" y2="13" />
      <line x1="12" y1="17" x2="12.01" y2="17" />
    </svg>
  );
}

function CatalogPanel({
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
    <section className="panel">
      <div className="panel-head">
        <h2>Catalog</h2>
        <span className="count">{products.length}</span>
      </div>
      <div className="panel-body">
        <div className="row">
          <input placeholder="SKU" value={sku} onChange={(e) => setSku(e.target.value)} />
          <input placeholder="Name" value={name} onChange={(e) => setName(e.target.value)} />
          <input
            placeholder="Price"
            type="number"
            step="0.01"
            value={price}
            onChange={(e) => setPrice(e.target.value)}
            style={{ maxWidth: 92 }}
          />
          <button onClick={create} disabled={busy || !sku || !name}>
            Add product
          </button>
        </div>

        <div className="table-wrap">
          <table>
            <thead>
              <tr>
                <th style={{ width: 44 }}>ID</th>
                <th>SKU</th>
                <th>Name</th>
                <th className="num">Price</th>
              </tr>
            </thead>
            <tbody>
              {products.map((p) => (
                <tr key={p.id}>
                  <td className="id">{p.id}</td>
                  <td className="sku">{p.sku}</td>
                  <td>{p.name}</td>
                  <td className="num">${(p.price_cents / 100).toFixed(2)}</td>
                </tr>
              ))}
              {products.length === 0 && (
                <tr>
                  <td colSpan={4} className="empty">
                    No products yet — add one above.
                  </td>
                </tr>
              )}
            </tbody>
          </table>
        </div>
      </div>
    </section>
  );
}

function AnalyticsPanel({
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
  const kinds = summary?.kinds ?? [];
  const max = Math.max(1, ...kinds.map((k) => k.count));

  const send = async () => {
    setBusy(true);
    try {
      await api.ingest({ kind, product_id: products[0]?.id, props: { source: "web" } });
      onChange();
    } catch (e) {
      onError(String(e instanceof Error ? e.message : e));
    } finally {
      setBusy(false);
    }
  };

  return (
    <section className="panel">
      <div className="panel-head">
        <h2>Analytics</h2>
        <span className="count">{summary?.total ?? 0}</span>
      </div>
      <div className="panel-body">
        <div className="row">
          <input placeholder="event kind" value={kind} onChange={(e) => setKind(e.target.value)} />
          <button onClick={send} disabled={busy || !kind}>
            Ingest
          </button>
        </div>
        <div className="row" style={{ marginTop: 8 }}>
          {["view", "add_to_cart", "purchase"].map((k) => (
            <button key={k} className="ghost" onClick={() => setKind(k)}>
              {k}
            </button>
          ))}
        </div>

        <div className="meters">
          {kinds.map((k) => (
            <div className="meter" key={k.kind}>
              <span className="k">{k.kind}</span>
              <span className="track">
                <span className="fill" style={{ width: `${(k.count / max) * 100}%` }} />
              </span>
              <span className="v">{k.count}</span>
            </div>
          ))}
          {kinds.length === 0 && <div className="empty">No events yet — ingest one above.</div>}
        </div>
      </div>
    </section>
  );
}

CREATE TABLE IF NOT EXISTS demo_notes (
  id         bigserial PRIMARY KEY,
  title      text NOT NULL,
  body       text NOT NULL,
  priority   text NOT NULL,
  metadata   jsonb NOT NULL DEFAULT '{}',
  due_date   timestamptz,
  created_at timestamptz NOT NULL DEFAULT now()
);

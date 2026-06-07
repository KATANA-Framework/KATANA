-- wrk payload for POST /notes (SQL write path). Each request creates a note.
wrk.method = "POST"
wrk.headers["Content-Type"] = "application/json"
local prio = {"low","medium","high"}
request = function()
  local p = prio[math.random(1,3)]
  local body = string.format(
    '{"title":"load %d","body":"benchmark note","priority":"%s","metadata":{"src":"wrk"},"due_date":"2026-06-10T09:00:00Z"}',
    math.random(1,1000000), p)
  return wrk.format(nil, "/notes", nil, body)
end

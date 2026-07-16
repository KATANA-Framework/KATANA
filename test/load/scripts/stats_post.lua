-- Scenario: stats-128 — POST /compute/stats with 128 doubles + include_median.
-- Measures large JSON number-array parse + aggregation + JSON serialize.
-- The values array is deterministic and identical for every framework.

local parts = {}
for i = 1, 128 do
   parts[i] = string.format("%.2f", ((i * 37) % 1000) + i * 0.25)
end
local body = '{"values":[' .. table.concat(parts, ",") .. '],"include_median":true}'

wrk.method = "POST"
wrk.body = body
wrk.headers["Content-Type"] = "application/json"
wrk.headers["Accept"] = "application/json"

request = function()
   return wrk.format("POST", "/compute/stats", nil, body)
end

done = function(summary, latency, requests)
   local status_errors = (summary.errors and summary.errors.status) or 0
   local timeout_errors = (summary.errors and summary.errors.timeout) or 0
   local total_errors = 0
   if summary.errors then
      total_errors = (summary.errors.connect or 0)
         + (summary.errors.read or 0)
         + (summary.errors.write or 0)
         + (summary.errors.timeout or 0)
         + (summary.errors.status or 0)
   end

   io.write(string.format("KATANA_LATENCY_P50_US: %.3f\n", latency:percentile(50.0)))
   io.write(string.format("KATANA_LATENCY_P90_US: %.3f\n", latency:percentile(90.0)))
   io.write(string.format("KATANA_LATENCY_P95_US: %.3f\n", latency:percentile(95.0)))
   io.write(string.format("KATANA_LATENCY_P99_US: %.3f\n", latency:percentile(99.0)))
   io.write(string.format("KATANA_LATENCY_P999_US: %.3f\n", latency:percentile(99.9)))
   io.write(string.format("KATANA_LATENCY_MAX_US: %.3f\n", latency.max))
   io.write(string.format("KATANA_ERRORS_STATUS: %d\n", status_errors))
   io.write(string.format("KATANA_ERRORS_TIMEOUT: %d\n", timeout_errors))
   io.write(string.format("KATANA_ERRORS: %d\n", total_errors))
end

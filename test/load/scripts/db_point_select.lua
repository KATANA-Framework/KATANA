-- Scenario: db-point-select — GET /items/{id} (single-row SELECT + JSON response).
-- Ids walk the seeded 1..KATANA_BENCHMARK_API_ITEM_COUNT range with a
-- multiplicative stride so the access pattern is spread out but fully
-- deterministic and identical for every framework.

local item_count = tonumber(os.getenv("KATANA_BENCHMARK_API_ITEM_COUNT")) or 4096
local counter = 0

wrk.headers["Accept"] = "application/json"

request = function()
   counter = counter + 1
   -- 48271 is odd and coprime with power-of-two item counts, so the walk
   -- covers every id; the product stays far below 2^53 (LuaJIT double safety).
   local id = 1 + ((counter * 48271) % item_count)
   return wrk.format("GET", "/items/" .. id)
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

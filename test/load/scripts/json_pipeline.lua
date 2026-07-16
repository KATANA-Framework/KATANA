-- Scenario: json-pipe — GET /json with HTTP/1.1 pipelining.
-- KATANA_PIPELINE_DEPTH requests are concatenated into a single write,
-- mirroring the legacy hello_pipeline.lua stress mode but against the JSON
-- endpoint served by every framework's full application binary.

local pipeline_depth = tonumber(os.getenv("KATANA_PIPELINE_DEPTH")) or 10

wrk.headers["Accept"] = "application/json"
local req = wrk.format("GET", "/json")
local payload = string.rep(req, pipeline_depth)

function request()
   return payload
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

   io.write(string.format("KATANA_PIPELINE_DEPTH: %d\n", pipeline_depth))
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

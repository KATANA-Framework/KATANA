-- Scenario: db-insert-returning — POST /items (INSERT ... RETURNING + JSON response).
-- Every request creates a new row and the server must return the created row
-- (including its generated id) as JSON. Payload shape is identical for every
-- framework; names are unique per thread/request.

local counter = 0
local thread_id = 0
local thread_counter = 0

setup = function(thread)
   thread_counter = thread_counter + 1
   thread:set("thread_id", thread_counter)
end

wrk.method = "POST"
wrk.headers["Content-Type"] = "application/json"
wrk.headers["Accept"] = "application/json"

request = function()
   counter = counter + 1
   local body = string.format(
      '{"name":"wrk-item-%d-%d","description":"benchmark insert returning path","price":19.99,"stock":5,"category":"tools"}',
      thread_id,
      counter
   )
   local headers = {
      ["Content-Type"] = "application/json",
      ["Accept"] = "application/json",
      ["X-Request-Id"] = string.format("550e8400-e29b-41d4-a716-%04d%08d", thread_id % 10000, counter % 100000000)
   }
   return wrk.format("POST", "/items", headers, body)
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

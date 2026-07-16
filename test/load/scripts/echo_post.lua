-- Scenario: echo-json — POST /echo with JSON body {message, repeat, uppercase}.
-- Measures JSON body parse + string transform + JSON serialize.
-- Request body is fixed; response message is 64 * 8 = 512 bytes of uppercased text.

local message = string.rep("abcdefgh", 8) -- 64 ASCII characters
local body = string.format('{"message":"%s","repeat":8,"uppercase":true}', message)

wrk.method = "POST"
wrk.body = body
wrk.headers["Content-Type"] = "application/json"
wrk.headers["Accept"] = "application/json"

request = function()
   return wrk.format("POST", "/echo", nil, body)
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

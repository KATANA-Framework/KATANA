local pipeline_depth = tonumber(os.getenv("KATANA_PIPELINE_DEPTH")) or 20

wrk.method = "POST"
wrk.body = "[1,2,3,4,5,6,7,8]"
wrk.headers["Content-Type"] = "application/json"
wrk.headers["Accept"] = "application/json"

local req = wrk.format("POST", "/compute/sum", nil, wrk.body)
local payload = string.rep(req, pipeline_depth)

function request()
   return payload
end

done = function(summary, latency, requests)
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
   io.write(string.format("KATANA_LATENCY_P95_US: %.3f\n", latency:percentile(95.0)))
   io.write(string.format("KATANA_LATENCY_P99_US: %.3f\n", latency:percentile(99.0)))
   io.write(string.format("KATANA_LATENCY_P999_US: %.3f\n", latency:percentile(99.9)))
   io.write(string.format("KATANA_LATENCY_MAX_US: %.3f\n", latency.max))
   io.write(string.format("KATANA_ERRORS: %d\n", total_errors))
end

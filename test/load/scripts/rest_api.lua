local counter = 0

request = function()
   counter = counter + 1
   local path
   local method
   local body = nil

   local r = counter % 5

   if r == 0 then
      method = "GET"
      path = "/api/users"
   elseif r == 1 then
      method = "GET"
      path = "/api/users/1"
   elseif r == 2 then
      method = "GET"
      path = "/api/users/2"
   elseif r == 3 then
      method = "GET"
      path = "/api/health"
   else
      method = "POST"
      path = "/api/users"
      body = '{"name":"TestUser","email":"test@example.com"}'
   end

   if body then
      wrk.headers["Content-Type"] = "application/json"
      return wrk.format(method, path, nil, body)
   else
      return wrk.format(method, path)
   end
end

done = function(summary, latency, requests)
   io.write("------------------------------\n")
   io.write("Mixed REST API Workload Results\n")
   io.write("------------------------------\n")
   io.write(string.format("Duration:  %.2fs\n", summary.duration / 1000000))
   io.write(string.format("Requests:  %d\n", summary.requests))
   io.write(string.format("RPS:       %.2f\n", summary.requests / (summary.duration / 1000000)))
   io.write("\nLatency Distribution:\n")
   io.write(string.format("  50%%: %.2fms\n", latency:percentile(50) / 1000))
   io.write(string.format("  75%%: %.2fms\n", latency:percentile(75) / 1000))
   io.write(string.format("  90%%: %.2fms\n", latency:percentile(90) / 1000))
   io.write(string.format("  99%%: %.2fms\n", latency:percentile(99) / 1000))
end

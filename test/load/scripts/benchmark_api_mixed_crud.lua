local item_count = tonumber(os.getenv("KATANA_BENCHMARK_API_ITEM_COUNT")) or 4096
local counter = 0

wrk.headers["Accept"] = "application/json"

local function emit_latency(summary, latency)
   local total_errors = 0
   if summary.errors then
      total_errors = (summary.errors.connect or 0)
         + (summary.errors.read or 0)
         + (summary.errors.write or 0)
         + (summary.errors.timeout or 0)
         + (summary.errors.status or 0)
   end

   io.write(string.format("KATANA_LATENCY_P50_US: %.3f\n", latency:percentile(50.0)))
   io.write(string.format("KATANA_LATENCY_P95_US: %.3f\n", latency:percentile(95.0)))
   io.write(string.format("KATANA_LATENCY_P99_US: %.3f\n", latency:percentile(99.0)))
   io.write(string.format("KATANA_LATENCY_P999_US: %.3f\n", latency:percentile(99.9)))
   io.write(string.format("KATANA_LATENCY_MAX_US: %.3f\n", latency.max))
   io.write(string.format("KATANA_ERRORS: %d\n", total_errors))
end

request = function()
   counter = counter + 1
   local slot = counter % 10
   local id = 1 + (counter % item_count)

   if slot < 6 then
      return wrk.format("GET", "/items/" .. id)
   elseif slot < 8 then
      local offset = (counter * 20) % math.max(20, item_count / 2)
      return wrk.format("GET", "/items?limit=20&offset=" .. offset .. "&category=tools")
   elseif slot == 8 then
      local body = string.format(
         '{"name":"wrk-item-%d","description":"wrk create path","price":19.99,"stock":3,"category":"tools"}',
         counter
      )
      local headers = {
         ["Content-Type"] = "application/json",
         ["X-Request-Id"] = string.format("550e8400-e29b-41d4-a716-%012d", counter)
      }
      return wrk.format("POST", "/items", headers, body)
   end

   local body = string.format(
      '{"name":"wrk-update-%d","price":29.99,"stock":5}',
      counter
   )
   local headers = {
      ["Content-Type"] = "application/json"
   }
   return wrk.format("PUT", "/items/" .. id, headers, body)
end

done = function(summary, latency, requests)
   emit_latency(summary, latency)
end

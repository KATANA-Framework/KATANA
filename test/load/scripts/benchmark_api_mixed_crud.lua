local item_count = tonumber(os.getenv("KATANA_BENCHMARK_API_ITEM_COUNT")) or 4096
local counter = 0
local created_ids = {}
local created_head = 1
local created_tail = 0

wrk.headers["Accept"] = "application/json"

local function push_created_id(id)
   created_tail = created_tail + 1
   created_ids[created_tail] = id
end

local function pop_created_id()
   if created_head > created_tail then
      return nil
   end

   local id = created_ids[created_head]
   created_ids[created_head] = nil
   created_head = created_head + 1
   return id
end

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

   if slot < 5 then
      return wrk.format("GET", "/items/" .. id)
   elseif slot < 7 then
      local offset = (counter * 20) % math.max(20, item_count / 2)
      return wrk.format("GET", "/items?limit=20&offset=" .. offset .. "&category=tools")
   elseif slot == 7 then
      local body = string.format(
         '{"name":"wrk-item-%d","description":"wrk create path","price":19.99,"stock":3,"category":"tools"}',
         counter
      )
      local headers = {
         ["Content-Type"] = "application/json",
         ["X-Request-Id"] = string.format("550e8400-e29b-41d4-a716-%012d", counter)
      }
      return wrk.format("POST", "/items", headers, body)
   elseif slot == 8 then
      local body = string.format(
         '{"name":"wrk-update-%d","price":29.99,"stock":5}',
         counter
      )
      local headers = {
         ["Content-Type"] = "application/json"
      }
      return wrk.format("PUT", "/items/" .. id, headers, body)
   end

   local created_id = pop_created_id()
   if created_id ~= nil then
      return wrk.format("DELETE", "/items/" .. created_id)
   end

   return wrk.format("GET", "/items/" .. id)
end

response = function(status, headers, body)
   if status ~= 201 then
      return
   end

   local id = body:match('"id"%s*:%s*(%d+)')
   if id ~= nil then
      push_created_id(id)
   end
end

done = function(summary, latency, requests)
   emit_latency(summary, latency)
end

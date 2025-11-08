wrk.method = "POST"
wrk.body   = '{"key":"value","test":"data"}'
wrk.headers["Content-Type"] = "application/json"

init = function(args)
   requests  = 0
   responses = 0
end

request = function()
   requests = requests + 1
   return wrk.format(nil, nil, nil, wrk.body)
end

response = function(status, headers, body)
   responses = responses + 1
end

done = function(summary, latency, requests)
   io.write("------------------------------\n")
   io.write(string.format("Requests:  %d\n", requests))
   io.write(string.format("Responses: %d\n", responses))
   io.write(string.format("Duration:  %.2fs\n", summary.duration / 1000000))
   io.write(string.format("RPS:       %.2f\n", requests / (summary.duration / 1000000)))
end

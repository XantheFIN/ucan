-- local can = require "CanLua"

if can == nil then
	error("can module not loaded!")
end

if can_h ~= 0 then
	h = can_h
else	
    type = can.SocketCan
     
	adapters = can.init_adapter_type(type)
	
	print("Adapters discovered:")
	for index, data in ipairs(adapters) do
		print(data)
	end
	
	h = can.obtain_handle(type, "vcan0")
end

if not can.handle_exists(h) then
	error("Handle " .. h .. " does not exist!")
end

can.set_baudrate(h, 500000)
numf = can.get_number_of_filters(h)
print("Number of CAN filter: " .. numf)

can.set_acceptance_filter(h, 0,  0x778, 0x7FF, false)

if can.open(h) and can.go_bus_on(h) then
	local ok, msg = pcall(function ()
		print("CAN port open.")

		tx_err, rx_err = can.get_error_counters(h)
		print("Error counter: " .. tx_err .. "," .. rx_err)
				
		for i=1,2 do
			tid = can.send_message(h, {id = 0x770, data = "023E00FFFFFFFFFF", isext = false})			
			m = can.get_send_ackn_message(h, tid, 1000)
			if not (m == nil) then
				print(string.format("Sent: 0x%x %s %s", m.id, m.data, tostring(m.isext)))
			end
			
			m = can.get_received_message(h, 1000)
			if not (m == nil) then
				print(string.format("Received: 0x%x %s %s", m.id, m.data, tostring(m.isext)))
			end
		end
	end)

	if not ok then
		print("Error occurred: " .. msg)
	end
			
    can.go_bus_off(h)
	can.close(h)
	print("CAN port closed.")
else
	print("Failed opening CAN.")
	ec = can.get_error_code(h)
	print(can.get_error_description(h, ec))
end

if can_h == 0 then
	can.release_handle(h)
	can.release_all_handles()
end

require ("lxp.lom")
local url = require("socket.url")

local function parseTable(t)
	local result = "?"
	for k,v in pairs(t) do
		if type(k)=="number" and type(v)=="table" then
			if v["tag"]=="tk:text_result" then
				if v[1] then
					result = v[1]
				end
			end
			if v["tag"]=="tk:error_message"then
				result = v[1]
			end
		end
	end
	return result
end

plugin.query = function (target, from, arg)
	local account_id = "api_ezbox";
	local password = "xxxxxx";
	local query = string.format("https://api.trueknowledge.com/direct_answer?wikipedia,official&question=%s&api_account_id=%s&api_password=%s", url.escape(arg), account_id, password)
	local result, error, header = socket.http.request(query)
	if error==200 then
		local lomTable = lxp.lom.parse (result)
		result = parseTable(lomTable)
		irc.say(target,result)
	end	
end

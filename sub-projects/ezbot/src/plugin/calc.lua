--  Nice plugin that uses google's calculator
--  Google's calculator is fun!
--  How to use:
--  !calc <Operation>
--  Example:
--  !calc 1+1
--  !calc sin(0.5)
--  !calc speed of light / sin(0.5)

local url = require("socket.url")
json = require("json")

plugin.calc = function (target, from, arg)
    local query = string.format("http://www.google.com/search?num=1&q=%s", url.escape(arg))
	local result, error, header = socket.http.request(query)
	if not result then irc.say(target,"no result") return end
	local a,b,c,r = string.find(result,"<td nowrap ><h2 class=r style=(.+)><b>(.+)</b></h2><tr><td>")
	if not r then irc.say(target,"no result") return end
	r = string.gsub(r,"<([^>]*)>","")
	irc.say(target,r)
end

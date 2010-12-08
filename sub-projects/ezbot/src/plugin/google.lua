--  The google plugin, just type:
--  !google <stuff to search>

local urltool = require("socket.url")
local json = require("json")
local htmlEntities = {
	["&nbsp;"] = " ",
	["&#160;"]  = " ",
	["&quot;"] = '"',
	["&#34;"]  = '"',
	["&apos;"] = "'",
	["&#39;"]  = "'",
	["&amp;"] = "&",
	["&#38;"]  = "&",
	["&lt;"] = "<",
	["&#60;"] = "<",
    ["&gt;"] = ">",
	["&#62;"] = ">",
	["&iexcl;"] = "¡",
	["&#161;"] = "¡",
	["&acute;"] = "´",
	["&#180;"] = "´",
	["&Ntilde;"] = "Ñ",
	["&#209;"] = "Ñ",
	["&ntilde;"] = "ñ",
	["&#241;"] = "ñ",
 }

local function parseHtmlEntites(s)
	local ret =string.gsub(s,"&.-;", function(input) return htmlEntities[input] or "?" end)
	return ret
end

plugin.google = function (target, from, arg)
    local search = urltool.escape(arg)
    local result, error, header = socket.http.request( 'http://ajax.googleapis.com/ajax/services/search/web?v=1.0&q='..search.."&safe=off")
	
    if error == 200 and result then
		local jsonTable = json.decode(result)
		if (jsonTable.responseData.results[1]) then
			--unicode hax
			content = string.gsub(urltool.unescape(jsonTable.responseData.results[1].content), "u(%x%x%x%x)",
                function(c)
                    local status, result = pcall(string.char, "0x"..c)
                    if status then
                        return result
                    else
                        return "u"..c
                    end
                end)
			--parse html tags to irc tags
			content = string.gsub(content,"(<b>)", "")
			content = string.gsub(content,"(</b>)", "")
			content = string.gsub(content,"(<u>)", "")
			content = string.gsub(content,"(</u>)", "")
			content = string.gsub(content,"(<i>)", "")
			content = string.gsub(content,"(</i>)", "")
			result = urltool.unescape(jsonTable.responseData.results[1].url).." "..content
			irc.say(target, parseHtmlEntites(result))
		end
    end
end

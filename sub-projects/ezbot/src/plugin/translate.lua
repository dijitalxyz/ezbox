--  Translate plugin, uses google translator. Translate text from any
--  language (spoken by google), to english.
--  How to use:
--  !translate <text in other language than english>



local url = require("socket.url")
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

local translateFunction = function (target, from, arg)
    local search = url.escape(arg)
    local result, error, header = socket.http.request('http://ajax.googleapis.com/ajax/services/language/translate?v=1.0&q=' ..  search .. '&langpair=%7Cen')
    local jsonTable = json.decode(result)
	if jsonTable.responseStatus == 200 then
		--unicode hax
		local translation = url.unescape(string.gsub(jsonTable.responseData.translatedText, "u(%x%x%x%x)",
            function(c)
                local status, result = pcall(string.char, "0x"..c)
                if status then
                    return result
                else
                    return "u"..c
                end
            end))
    result = "[lang: "..jsonTable.responseData.detectedSourceLanguage.."]: "..translation
        irc.say(target, parseHtmlEntites(result))
    end
end 

plugin.translate = translateFunction

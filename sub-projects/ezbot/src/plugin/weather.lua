--  The google weather plugin, just type:
--  !weather <location>
require ("lxp.lom")
local url = require("socket.url")
require('socket.http')

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

local function parseTable(t, location, city)
	local result = ""
    local resulttable = {}
    resulttable.forcast_conditions = {}
	for k,v in pairs(t) do
		if type(k)=="number" and type(v)=="table" then
            if v["tag"]=="current_conditions" then
                for i, t in pairs(v) do
                    --print(i,t.tag)
                    if t.tag =="condition" then
                        resulttable.condition = t.attr.data
                    end
                    if t.tag =="temp_f" then
                        resulttable.temp_f = t.attr.data
                    end
                    if t.tag =="temp_c" then
                        resulttable.temp_c = t.attr.data
                    end
                    if t.tag =="humidity" then
                        resulttable.humidity = t.attr.data
                    end
                    if t.tag =="wind_condition" then
                        resulttable.wind_condition = t.attr.data
                    end
                end
            end
            if v["tag"]=="forecast_conditions" then
                table.insert(resulttable.forcast_conditions,
                            "  |  "..v[1].attr.data..": Low:"..v[2].attr.data.."°F, High:"..v[3].attr.data.."°F, "..
                            "Condition: "..v[5].attr.data)
            end
            
		end
	end
    result = "Weather for "..city..": Temp:"..resulttable.temp_f.."°F/"..resulttable.temp_c.."°C, "..resulttable.humidity..", "..
              "Condition: "..resulttable.condition..", "..resulttable.wind_condition
    for k,v in pairs(resulttable.forcast_conditions) do
        result = result..v
    end
	return result
end

plugin.weather = function (target, from, arg)
    local query = "http://www.google.com/ig/api?weather="..url.escape(arg)
	local result, error, header = socket.http.request(query)
	if error==200 then
        if result:find("problem_cause") then
            irc.say(target, "Unable to get weather for "..arg..".")
            return
        end
        city = result:match('city data=".-"'):gsub('city data="(.-)"',"%1")
        result = result:gsub(city," ")
        postal_code = result:match('postal_code data=".-"'):gsub('postal_code data="(.-)"',"%1")
        result = result:gsub(postal_code," ")
        local lomTable = lxp.lom.parse (result)
		result = parseTable(lomTable[1], arg, city)
        irc.say(target, result)
	end	
end

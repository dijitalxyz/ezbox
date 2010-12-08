--  Short command hax
--  Given a short command, this plugin search in the plugin table, try to match a command
--  that "looks like" the short command, and execute the command.
--  for example:
--     "!gl test" will execute "!google test"
--     "!late hola" will execute "!translate hola"
--
--  The short command must be unique, !le will not work, because there is "le" in "google"
--  and "translate".
--  Also, the characters of short command must be in the same secuence of the command.
--  for example !gle will call !google, but !gel will do nothing.

local function isShortCommand(command,shortcommand)
    if type(command) == "string" and type(shortcommand) == "string" and shortcommand:len() > 0 then
        local pos = {}
        local char = ""
        local p = 0
        for i=1,shortcommand:len() do
            char = shortcommand:sub(i,i)
            p = command:find(char)
            if not p then
                return false
            end
            command = command:gsub(char," ",1)
            table.insert(pos,p)
        end
        for i=1,#pos-1 do
            if pos[i]>pos[i+1] then 
                return false 
            end
        end
    else
        return false
    end
    return true
end

callback.shortCommand = function(target, from, message)
    local is_cmd, cmd, arg = message:match("^(!)(%w+) (.*)$")
    if is_cmd and cmd then
        local command = nil
        for k,v in pairs(plugin) do
            if k == cmd then
                return
            end
            if isShortCommand(k,cmd) then
                if not command then
                    command = v
                else
                    return
                end
            end
        end
        if type(command)=="function" then
            command(target,from, arg)
        end
    end
end

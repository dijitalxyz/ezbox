--------------------------------------------------
-- Writen by martin_bfg10k (martinbfg10@gmail.com)
-- Modified by Zeta Labs (zetalabs@gmail.com)
--------------------------------------------------

local irc = require 'irc'
require('socket.http')


plugin = {}
callback = {}

require('config')

irc.DEBUG = true

irc.register_callback("connect", function()
    for i,v in ipairs(defaulChannels) do
        irc.join(v)
    end
end)

irc.register_callback("channel_msg", function(channel, from, message)
    local is_cmd, cmd, arg = message:match("^(!)(%w+) (.*)$")
    if is_cmd and plugin[cmd] then
        plugin[cmd](channel.name, from, arg)
    end
    for k,v in pairs(callback) do
        if type(v) == "function" then
            v(channel.name, from, message)
        end
    end
end)

irc.register_callback("private_msg", function(from, message)
    local is_cmd, cmd, arg = message:match("^(!)(%w+) (.*)$")
    if is_cmd and plugin[cmd] then
        plugin[cmd](from, from, arg)
    end
    for k,v in pairs(callback) do
        if type(v) == "function" then
            v(from, from, message)
        end
    end
end)
--[[
irc.register_callback("nick_change", function(from, old_nick)
end)
--]]
irc.connect{network = network, port = port, nickList = nickList, username = username, realname = realname}

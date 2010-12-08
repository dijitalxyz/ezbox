-- usage:
--      !join <channel>
--      example: !join #lua
plugin.join = function (target, from, arg)
        irc.join(arg)
end
